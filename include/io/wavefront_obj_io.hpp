#ifndef WAVEFRONT_OBJ_IO_HPP
#define WAVEFRONT_OBJ_IO_HPP

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <gromacs/math/vec.h>
#include <gromacs/utility/real.h>   


/*!
 * Abstract data type for faces in Wavefront OBJ objects. Faces are sets of 
 * vertex indices and (optionally) vertex normal indices.
 */
class WavefrontObjFace
{
    public:

        // constructors:
        WavefrontObjFace(
                const std::vector<int> &vertexIdx);
        WavefrontObjFace(
                const std::vector<int> &vertexIdx, 
                const std::vector<int> &normalIdx);

        // getter methods:
        int numVertices() const;
        int vertexIdx(int i) const;
        int normalIdx(int i) const;
        bool hasNormals() const;


//    private:

        // data container for indices:
        std::vector<int> vertexIdx_;
        std::vector<int> normalIdx_;
};


/*!
 * Effectively an abstract data type representing groups OBJ files. Consists
 * of a group name and a vector of integer vectors representing the list of 
 * faces.
 */
class WavefrontObjGroup
{
    public:

        // constructor:
        WavefrontObjGroup(std::string name);
        WavefrontObjGroup(const WavefrontObjGroup &other);

        // add face to group:
        void addFace(WavefrontObjFace face);

        // data:
        std::string groupname_;
        std::vector<WavefrontObjFace> faces_;
};


/*!
 * An abstract data type representing a complete Wavefront OBJ object (i.e. an
 * entire file). Consists of a vector of vertex positions and a vector of
 * WavefrongObjGroup objects.
 */
class WavefrontObjObject
{
    public:

        // constructor:
        WavefrontObjObject(std::string name);

        // functions to add data:
        void addVertices(
                const std::vector<gmx::RVec> &vertices);
        void addVertices(
                const std::vector<std::pair<gmx::RVec, real>> &vertices);
        void addVertexNormals(
                const std::vector<gmx::RVec> &normals);
        void addGroup(
                const WavefrontObjGroup &group);

        // returns flag indicating whether object is valid:
        bool valid() const;

        // functions to manipulate data:
        void scale(real fac);
        void shift(gmx::RVec shift);

        // functions to query data:
        gmx::RVec calculateCog();

        // data:
        std::string name_;
        std::vector<std::pair<gmx::RVec, real>> vertices_;
        std::vector<gmx::RVec> normals_;
        std::vector<WavefrontObjGroup> groups_;
};


/*!
 * Simple serialiser for writing data to a Wavefront OBJ geometry file. 
 * Currently, only writing of comments, vertices and faces is supported. Faces
 * may be grouped together. Does not perform error checking.
 */
class WavefrontObjExporter
{
    public:

        // interface for export:
        void write(std::string fileName,
                   std::vector<gmx::RVec> vertices,
                   std::vector<std::vector<int>> faces);

        void write(std::string fileName,
                   WavefrontObjObject object);


    private:

        // file handle:
        std::fstream obj_;

        // utilities for writing individual lines:
        inline void writeComment(std::string comment);
        inline void writeGroup(std::string group);
        inline void writeVertex(std::pair<gmx::RVec, real> vertex);
        inline void writeVertexNormal(gmx::RVec norm);
        inline void writeFace(const WavefrontObjFace &face);
};

#endif

