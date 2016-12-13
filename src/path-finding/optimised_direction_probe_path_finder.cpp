#include <iostream>

#include "path-finding/optimised_direction_probe_path_finder.hpp"

/*
 * Constructor.
 */
OptimisedDirectionProbePathFinder::OptimisedDirectionProbePathFinder(
        real probeStepLength,
        real probeRadius,
        real maxFreeDist,
        int maxProbeSteps,
        gmx::RVec initProbePos,
        std::vector<real> vdwRadii,
        gmx::AnalysisNeighborhoodSearch *nbSearch,
        int saRandomSeed,
        int saMaxCoolingIter,
        int saNumCostSamples,
        real saXi,
        real saConvRelTol,
        real saInitTemp,
        real saCoolingFactor,
        real saStepLengthFactor,
        bool saUseAdaptiveCandGen)
    : AbstractProbePathFinder(probeStepLength, probeRadius, maxFreeDist, 
                              maxProbeSteps, initProbePos, vdwRadii, nbSearch,
                              saRandomSeed, saMaxCoolingIter, saNumCostSamples,
                              saXi, saConvRelTol, saInitTemp, saCoolingFactor,
                              saStepLengthFactor, saUseAdaptiveCandGen)
{

}


/*
 *
 */
void
OptimisedDirectionProbePathFinder::findPath()
{
    std::cout<<"Yo!"<<std::endl;

/*
    // optimise initial position:
    optimiseInitialPos();
    
    // advance forward:
    advanceAndOptimise(true);

    // revert array:
    std::reverse(path_.begin(), path_.end());
    std::reverse(radii_.begin(), radii_.end());
    
    // advance backward:
    advanceAndOptimise(false);
    */
}


/*
 *
 */
void
OptimisedDirectionProbePathFinder::optimiseInitialPos()
{
/*
    // set current probe position to initial probe position: 
    crntProbePos_ = initProbePos_;

    std::cout<<"crntProbePos = "<<crntProbePos_[0]<<"  "
                                <<crntProbePos_[1]<<"  "
                                <<crntProbePos_[2]<<"  "
             <<"initProbePos = "<<initProbePos_[0]<<"  "
                                <<initProbePos_[1]<<"  "
                                <<initProbePos_[2]<<"  "
             <<"orthVecU = "<<orthVecU_[0]<<"  "
                            <<orthVecU_[1]<<"  "
                            <<orthVecU_[2]<<"  "
             <<"orthVecW = "<<orthVecW_[0]<<"  "
                            <<orthVecW_[1]<<"  "
                            <<orthVecW_[2]<<"  "
              <<std::endl;

    // initial state in optimisation space is always null vector:
    int stateDim = 2;
    real initState[stateDim] = {0.0, 0.0};
    
    // cost function is minimal free distance function:
    costFunction cf;
    cf = std::bind(&InplaneOptimisedProbePathFinder::findMinimalFreeDistance, 
                   this, std::placeholders::_1);
   
    // create new simulated annealing module:
    SimulatedAnnealingModule sam(stateDim, saRandomSeed_, saMaxCoolingIter_,
                                 saNumCostSamples_, saXi_, saConvRelTol_, 
                                 saInitTemp_, saCoolingFactor_, 
                                 saStepLengthFactor_, initState, cf,
                                 saUseAdaptiveCandGen_);
    
    // perform simulated annealing:
    eSimAnTerm status = sam.anneal();
    
    // set initial position to its optimal value:
    initProbePos_ = optimToConfig(sam.getBestState());


    std::cout<<"initProbePos = "<<initProbePos_[0]<<"  "
                                <<initProbePos_[1]<<"  "
                                <<initProbePos_[2]<<"  "
             <<"status = "<<status<<"  "
             <<"bestState = "<<sam.getBestState()[0]<<" "
                             <<sam.getBestState()[1]<<" "
             <<std::endl;
    std::cout<<"crntProbePos = "<<crntProbePos_[0]<<"  "
                                <<crntProbePos_[1]<<"  "
                                <<crntProbePos_[2]<<"  "
             <<std::endl; 

    // add path support point and associated radius to container:
    path_.push_back(initProbePos_);
    radii_.push_back(sam.getBestCost());
*/
}


/*
 *
 */
void
OptimisedDirectionProbePathFinder::advanceAndOptimise(bool forward)
{
/*
    // set previous position to initial point:
    crntProbePos_ = initProbePos_;

    // set up direction vector for forward/backward marching:
    gmx::RVec direction(chanDirVec_);
    if( !forward )
    {
        direction[0] = -direction[0];
        direction[1] = -direction[1];
        direction[2] = -direction[2];
    }

    // initial state in optimisation space is always null vector:
    int stateDim = 2;
    real initState[stateDim] = {0.0, 0.0};

    // cost function is minimal free distance function:
    costFunction cf;
    cf = std::bind(&InplaneOptimisedProbePathFinder::findMinimalFreeDistance, 
                   this, std::placeholders::_1);

    // advance probe in direction of (inverse) channel direction vector:
    int numProbeSteps = 0;
    while(true)
    {
        std::cout<<"probeStepLength = "<<probeStepLength_<<std::endl;
        std::cout<<"direction = "<<direction[0]<<"  "
                                 <<direction[1]<<"  "
                                 <<direction[2]<<"  "
                 <<"crntProbePos = "<<crntProbePos_[0]<<"  "
                                    <<crntProbePos_[1]<<"  "
                                    <<crntProbePos_[2]<<"  "
                 <<std::endl;

        // advance probe position to next plane:
        crntProbePos_[0] = crntProbePos_[0] + probeStepLength_*direction[0];
        crntProbePos_[1] = crntProbePos_[1] + probeStepLength_*direction[1];
        crntProbePos_[2] = crntProbePos_[2] + probeStepLength_*direction[2]; 

        // create new simulated annealing module:
        SimulatedAnnealingModule sam(stateDim, saRandomSeed_, saMaxCoolingIter_,
                                     saNumCostSamples_, saXi_, saConvRelTol_,
                                     saInitTemp_, saCoolingFactor_,
                                     saStepLengthFactor_, initState, cf,
                                     saUseAdaptiveCandGen_);

        // optimise in plane:
        eSimAnTerm status = sam.anneal();
          
        // current position becomes best position in plane: 
        crntProbePos_ = optimToConfig(sam.getBestState());
        
        // add result to path container: 
        path_.push_back(crntProbePos_);
        radii_.push_back(sam.getBestCost());     
              
       
        if( numProbeSteps >= maxProbeSteps_ )
        {
            break;
        }
        if( sam.getBestCost() > maxProbeRadius_ )
        {
            break;
        }
    }
*/
}


/*
 * TODO: implement this fully!
 */
gmx::RVec
OptimisedDirectionProbePathFinder::optimToConfig(real *optimSpacePos)
{

    // get configuration space position via orthogonal vectors:
    gmx::RVec configSpacePos;/*
    configSpacePos[0] = crntProbePos_[0] + optimSpacePos[0]*orthVecU_[0]
                                         + optimSpacePos[1]*orthVecW_[0];
    configSpacePos[1] = crntProbePos_[1] + optimSpacePos[0]*orthVecU_[1]
                                         + optimSpacePos[1]*orthVecW_[1];
    configSpacePos[2] = crntProbePos_[2] + optimSpacePos[0]*orthVecU_[2] 
                                         + optimSpacePos[1]*orthVecW_[2];
    */
    // return configuration space position:
    return(configSpacePos);
    
}

