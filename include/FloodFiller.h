#ifndef INCLUDE_FLOODFILLER_H_
#define INCLUDE_FLOODFILLER_H_

#include "dealIIheaders.h"

/**
* This class holds information for a grain, including its index and the list of vertices in that grain.
*/
template <int dim>
class GrainSet
{
public:
    /**
    * Sets the grain index.
    */
    void setGrainIndex(unsigned int _grain_index){grain_index = _grain_index;};

    /**
    * Gets the grain index.
    */
    unsigned int getGrainIndex(){return grain_index;};

    /**
    * Adds the vertices of a new element to the list.
    */
    void addVertexList(std::vector<dealii::Point<dim>> _vertices){list_of_vertices.push_back(_vertices);};

    /**
    * Gets the entire list of elements and the list of vertices per element.
    */
    std::vector<std::vector<dealii::Point<dim>>> getVertexList(){return list_of_vertices;};

private:
    /**
    * The grain index.
    */
    unsigned int grain_index;

    /**
    * A vector of the elements in the grain containing a vector of the vertices for each element.
    */
    std::vector<std::vector<dealii::Point<dim>>> list_of_vertices;
};



/**
* This class uses a recursive flood filling algorithm to find connected bodies in a field, given a threshold. The MPI communication methods are similar to those in parallelNucleationList.
*/
template <int dim, int degree>
class FloodFiller
{
public:
    /**
    * Constructor.
    */
    FloodFiller(FESystem<dim> & _fe, QGaussLobatto<dim> _quadrature): quadrature(_quadrature), num_quad_points(_quadrature.size()), dofs_per_cell(_fe.dofs_per_cell){
        fe = & _fe;
    };

    /**
    * The primary external interface. This method takes in information about the mesh/field and outputs a vector of GrainSet objects.
    */
    void calcGrainSets(FESystem<dim> & fe, dealii::DoFHandler<dim> &dof_handler, vectorType* solution_field, double threshold, std::vector<GrainSet<dim>> & grain_sets);
protected:

    /**
    * The actual recursive flood fill method.
    */
    template <typename T>
    void recursiveFloodFill(T di, T di_end, vectorType* solution_field, double threshold, unsigned int & grain_index, std::vector<GrainSet<dim>> & grain_sets, bool & grain_assigned);

    /**
    * Method to communicate and merge the local representations of the grains into a global list.
    */
    void communicateGrainSets(std::vector<GrainSet<dim>> & grain_sets);

    /**
    * The method to send the grain_sets to the next processor on the list.
    */
    void sendUpdate (int procno, std::vector<GrainSet<dim>> & grain_sets) const;

    /**
    * The method to recieve the grain_sets from the previous processor on the list.
    */
    void receiveUpdate (int procno, std::vector<GrainSet<dim>> & grain_sets) const;

    /**
    * The method to send the global grain_sets to all processors on the list.
    */
    void broadcastUpdate (int broadcastProc, int thisProc, std::vector<GrainSet<dim>> & grain_sets) const;

    /**
    * Checks to see if grains found on different processors are parts of a larger grain. If so, it merges the grain_sets entries.
    */
    void mergeSplitGrains (std::vector<GrainSet<dim>> & grain_sets) const;

    /**
    * The quadrature used to calculate the element-wise value of the solution field.
    */
    QGaussLobatto<dim>  quadrature;

    /**
    * The number of quadrature points per cell.
    */
    const unsigned int   num_quad_points;

    /**
    * The number of degrees of freedom per cell.
    */
    const unsigned int   dofs_per_cell;

    /**
    * The deal.II finite element object, set in the constructor.
    */
    FESystem<dim> * fe;
};

#endif
