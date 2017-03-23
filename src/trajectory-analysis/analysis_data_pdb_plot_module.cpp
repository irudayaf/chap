#include <iostream>
#include <string>
#include <cstring>
#include <iomanip>

#include <gromacs/analysisdata/dataframe.h>

#include "trajectory-analysis/analysis_data_pdb_plot_module.hpp"


/*
 *
 */
AnalysisDataPdbPlotModule::AnalysisDataPdbPlotModule()                                
    : precision_(5)
{

}                                           


/*
 *
 */
AnalysisDataPdbPlotModule::AnalysisDataPdbPlotModule(int i)
    : precision_(5)
{

}                                                                               


/*
 *
 */
int
AnalysisDataPdbPlotModule::flags() const
{
    return efAllowMissing | efAllowMulticolumn | efAllowMultipoint
           | efAllowMultipleDataSets; 
}


/*
 *
 */
void                                                                            
AnalysisDataPdbPlotModule::pointsAdded(const gmx::AnalysisDataPointSetRef &points)      
{
    // check that file is open:
    if( file_.is_open() == true )
    {
        // write pore particle positions to PDB file:
        for(unsigned int i = 0; i < 1; i++)                                   
        {
            file_<<std::setw(6)<<"ATOM  "                       // record name             
                 <<std::setw(5)<<i+1                            // atom serial number (one-based)
                 <<std::setw(1)<<" "                                                 
                 <<std::setw(4)<<"PORE"                         // atom name                    
                 <<std::setw(1)<<" "                            // alternate location indicator
                 <<std::setw(3)<<"POR"                          // residue name              
                 <<std::setw(1)<<""                                                  
                 <<std::setw(1)<<"X"                            // chain identifier             
                 <<std::setw(4)<<"000"                          // residue sequence number   
                 <<std::setw(1)<<" "                            // code for insertion of residues
                 <<std::setw(3)<<""                                                  
                 <<std::setw(8)<<points.values()[0].value()*10  // x [Ang]                   
                 <<std::setw(8)<<points.values()[1].value()*10  // y [Ang]                   
                 <<std::setw(8)<<points.values()[2].value()*10  // z [Ang]                   
                 <<std::setw(6)<<points.values()[4].value()*10  // occupancy -> radius [Ang]              
                 <<std::setw(6)<<points.values()[4].value()*10  // temperature factor -> radius [Ang]  
                 <<std::setw(10)<<""                                                 
                 <<std::setw(2)<<"XX"                           // element symbol            
                 <<std::setw(2)<<0                              // charge                    
                 <<std::endl;
        }
    }
} 


/*
 * Opens filestream for output writing.
 */
void 
AnalysisDataPdbPlotModule::dataStarted(gmx::AbstractAnalysisData *data)
{

}


/*
 * 
 */
void 
AnalysisDataPdbPlotModule::frameStarted(const gmx::AnalysisDataFrameHeader &frame)
{
    // make sure that file name is given:
    if( strlen(fileName_) > 0 )
    {
        // append frame number to file name:
        int frameIndex = frame.index();
        std::string sepString = ".";
        std::string frameFileName = fileName_ + sepString + std::to_string(frameIndex); 
       
        // open file stream and delete any prior content:
        file_.open(frameFileName, std::fstream::out | std::fstream::trunc);

        // set precision:
        file_.precision(3);
    }

}


/*
 * 
 */
void 
AnalysisDataPdbPlotModule::frameFinished(const gmx::AnalysisDataFrameHeader &frame)
{
    // add box size to PDB file:
    // TODO: is this necessary?
//    file_<<"  10.34380  10.34380  10.83500"<<std::endl; 
   
    // close file stream:
    file_.close();
}


/*
 * Closes the file stream when all data has been written.
 */
void 
AnalysisDataPdbPlotModule::dataFinished()
{

}



