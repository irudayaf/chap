#include <algorithm>
#include <iostream>

//#include <gromacs/topology/atomprop.h> 
#include <gromacs/topology/atoms.h>  
#include <gromacs/topology/topology.h>  

#include "path-finding/vdw_radius_provider.hpp"


/*!
 * \brief Constructor.
 *
 * Initialises defRad_ as -1.0.
 */
VdwRadiusProvider::VdwRadiusProvider()
    : defRad_(-1.0)
{

}

/*!
 * \brief 
 * Destructor.
 */
VdwRadiusProvider::~VdwRadiusProvider()
{

}


/*!
 *  \brief Setter function for defRad_ member.
 *
 *  Unless this function is called prior to lookupTableFromJson(), defRad_ will
 *  be -1.0 and an exception is thrown if VdwRadiusProvider can not find a 
 *  record for a specific atom and residue name combination.
 *
 *  Note that defRad_ can only be set to values greater or equal zero. Any 
 *  attempt at setting a negative value will cause an exception to be thrown.
 */
void
VdwRadiusProvider::setDefaultVdwRadius(real defRad)
{
    if( defRad >= 0 )
    {
        defRad_ = defRad;
    }
    else
    {
        throw std::runtime_error("Default van der Waals radius may not be negative.");
    }
}


/*
 *
 */
void
VdwRadiusProvider::lookupTableFromJson(rapidjson::Document &jsonDoc)
{
    // ensure that root of JSON is object:
    if( jsonDoc.IsObject() == false )
    {
        throw std::runtime_error("No valid JSON object provided.");
    }

    // ensure that JSON contains vdwradii array:
    if( jsonDoc.HasMember("vdwradii") == false || jsonDoc["vdwradii"].IsArray() == false )
    {
        throw std::runtime_error("Provided JSON does not contain vdwradii array.");
    }

    // extract vdw radius data:
    const rapidjson::Value &vdwRadiiEntries = jsonDoc["vdwradii"];

    // prepare loopup table for vdW radii:
    vdwRadiusLookupTable_.clear();

    // loop over array and extract vdW radius lookup table:
    rapidjson::Value::ConstValueIterator it;
    for(it = vdwRadiiEntries.Begin(); it != vdwRadiiEntries.End(); it++)
    {
        // check that required entries are present and have correct type:
        if( it -> HasMember("atomname") == false || (*it)["atomname"].IsString() == false )
        {
            throw std::runtime_error("No 'atomname' attribute of type 'string' in van der Waals radius record.");
        }
        if( it -> HasMember("resname") == false || (*it)["resname"].IsString() == false )
        {
            throw std::runtime_error("No 'resname' attribute of type 'string' in van der Waals radius record.");
        }
        if( it -> HasMember("vdwr") == false || (*it)["vdwr"].IsNumber() == false )
        {
            throw std::runtime_error("No 'vdwr' attribute of type 'number' in van der Waals radius record.");
        }

        // create a vdW radius record struct from JSON:
        VdwRadiusRecord rec;
        rec.atmName_ = (*it)["atomname"].GetString();
        rec.resName_ = (*it)["resname"].GetString();
        rec.vdwRad_ = (*it)["vdwr"].GetDouble();

        // add record to lookup table:
        vdwRadiusLookupTable_.push_back(rec);
    } 
}


/*
 *
 */
std::unordered_map<int, real>
VdwRadiusProvider::vdwRadiiForTopology(const gmx::TopologyInformation &top,
                                       gmx::ConstArrayRef<int> mappedIds)
{
    // get list of all atoms:
    t_atoms atoms = top.topology() -> atoms;

    // allocate memory for results vector:
    std::unordered_map<int, real> vdwRadii;
    vdwRadii.reserve(atoms.nr);

    // loop over all atoms in topology and find vdW radii:
    for(size_t i = 0; i < mappedIds.size(); i++)
    {
        // get atom and residue names:
        std::string atmName = *(atoms.atomname[mappedIds[i]]);
        std::string resName = *(atoms.resinfo[atoms.atom[mappedIds[i]].resind].name);
        std::string elemSym(atoms.atom[mappedIds[i]].elem);

        // add radius for this atom to results vector:
        vdwRadii[mappedIds[i]] = vdwRadiusForAtom(atmName, resName, elemSym);
    }

    // return vector of vdW radii:
    return vdwRadii;
}


/*!
 * \brief Driver for van der Waals radius lookups.
 *
 * Given a combination of atom name, residue name, and element, name, this 
 * function tries return the corresponding van der Waals radius. If 
 * setDefaultVdwRadius() has not been called, an exception will be thrown is
 * no match is found in the internal lookup table.
 */
real
VdwRadiusProvider::vdwRadiusForAtom(std::string atmName, 
                                    std::string resName,
                                    std::string elemSym)
{
    // build vector of atom name matches:
    std::vector<VdwRadiusRecord> atmNameMatches = matchAtmName(atmName);

    // have any atom name matches been found:
    if( atmNameMatches.size() > 0 )
    {
        // try to find exact residue name match:
        std::vector<VdwRadiusRecord>::const_iterator it;
        it = matchResName(resName, atmNameMatches);

        // handle case where no match was found:
        if( it == atmNameMatches.end() )
        {
            // try to find generic residue match:
            it = matchResName("???", atmNameMatches);

            // any matches found?
            if( it == atmNameMatches.end() )
            {
                // if no match, return default radius:
                return returnDefaultRadius(atmName, resName);
            }
            else
            {
                // if match found return corresponding radius:
                return(it -> vdwRad_);
            }
        }
        else
        {
            return(it -> vdwRad_);
        }
    }
    else
    {
        // try to find element name matches:
        std::transform(elemSym.begin(), elemSym.end(), elemSym.begin(), ::toupper);
        std::vector<VdwRadiusRecord> elemNameMatches = matchAtmName(elemSym);

        // any matches found?
        if( elemNameMatches.size() > 0 )
        {
            // try to find exact residue name match:
            std::vector<VdwRadiusRecord>::const_iterator it;
            it = matchResName(resName, elemNameMatches);

            // handle case where no match was found:
            if( it == elemNameMatches.end() )
            {
                // try to find generic residue match:
                it = matchResName("???", elemNameMatches);

                // any matches found?
                if( it == elemNameMatches.end() )
                {
                    // if no match, return default radius:
                    return returnDefaultRadius(atmName, resName);
                }
                else
                {
                    // if match found return corresponding radius:
                    return(it -> vdwRad_);
                }
            }
            else
            {
                return(it -> vdwRad_);
            }
        }
        else
        {
            // if no match found, try default radius:
            return returnDefaultRadius(atmName, resName);
        }
    }
}


/*!
 * \brief Internal utility function for matching atom names.
 *
 * Searches internal lookup table for records with matching atom names and 
 * returns a vector of all such records. If no matching record was found, the
 * returned vector will be empty.
 */
std::vector<VdwRadiusRecord>
VdwRadiusProvider::matchAtmName(std::string atmName)
{
    // build vector of atom name matches:
    std::vector<VdwRadiusRecord> matches;
    std::vector<VdwRadiusRecord>::iterator it;
    for(it = vdwRadiusLookupTable_.begin(); it != vdwRadiusLookupTable_.end(); it++)
    {
        if(it -> atmName_ == atmName )
        {
            matches.push_back(*it);
        }
    }

    // return vector of matches:
    return matches;
}


/*!
 * \brief Internal utility function for matching residue names.
 *
 * Searches the records vector for an element with matching residue name. If 
 * successful, the function returns an iterator pointing to the VdwRadiusRecord
 * with matching residue name. If no matching residue name can be found, the
 * iterator will point to the end of the vector.
 */
std::vector<VdwRadiusRecord>::const_iterator
VdwRadiusProvider::matchResName(std::string resName,
                                const std::vector<VdwRadiusRecord> &records)
{
    // loop over entries and try to match residue name:
    std::vector<VdwRadiusRecord>::const_iterator it;
    for(it = records.begin(); it != records.end(); it++)
    {
        if( it -> resName_ == resName )
        {
            return(it);
        }
    }

    // if no match was found, point iterator to end of vector:
    it = records.end();
    return(it);
}


/*!
 * \brief Internal utility function for returning the default radius.
 *
 * If setDefaultVdwRadius() has been called, this function returns the value
 * defRad_ has been set to. Otherwise it throws an exception.
 */
real
VdwRadiusProvider::returnDefaultRadius(std::string atmName, std::string resName)
{
    // check if default radius has been set:
    if( defRad_ >= 0 )
    {
        // just return the default radius:
        return(defRad_);
    }
    else
    {
        throw std::runtime_error("ERROR: Could not find van der Waals radius for atom with atom name "+atmName+" and residue name "+resName+" and default radius is not set.");
    } 
}

















