/*
  IsoSurfacer.h: Isosurface computation class.
  Copyright (C) 2013  Julien Tierny <tierny@telecom-paristech.fr>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    File modified by T. Vigier (January 2015)
*/


#ifndef ISOSURFACER_H
#define ISOSURFACER_H

#include  <map>
#include  <queue>

#include  "Debug.h"

#include  <vtkAlgorithm.h>
#include  <vtkCellArray.h>
#include  <vtkDoubleArray.h>
#include  <vtkObjectFactory.h>
#include  <vtkPointData.h>
#include  <vtkPolyData.h>
#include  <vtkUnstructuredGrid.h>

enum extractionType {SIMPLE, STANDARD, FAST};

class TetIndex{
 
  public:
    
    TetIndex(){
    };

	inline void SetBounds(double min, double max)
	{
		Fmin = min;
		Fmax = max;
	}

	inline void SetResolution(int size)
	{
		resolution = size;
		table.resize(resolution);
	}


	inline void AddTet(vtkIdType tetId, int minTet, int maxTet)
	{
		//QUESTION 18
		//WARNING: special case if isoValue = Fmax

	}

	inline vector<vtkIdType>* GetCandidates(double isoValue)
	{
		//QUESTION 19
		//what is the interval of the isovalue?
		//WARNING: special case if isoValue = Fmax

	}
    
	  vector< vector<vtkIdType> > table; 
	  int resolution; // table size
	  double Fmin, Fmax; //global function range
    
};

class EdgeIntersection {
  
  public:
	  pair<vtkIdType, vtkIdType> EdgeVertexIds;
	  vtkIdType CreatedVertex;
   
	   EdgeIntersection()
	   {
			
		}
	   
	   ~EdgeIntersection()
	   {
		   
	   }

	    inline void SetEdgeVertex(const pair<vtkIdType, vtkIdType>& ids)
	   {
		   EdgeVertexIds.first = ids.first;
		   EdgeVertexIds.second = ids.second;
	   }

	   inline void SetCreatedVertex(vtkIdType vert)
	   {
		   CreatedVertex = vert;
	   }

    
};

class VTK_EXPORT IsoSurfacer : public vtkAlgorithm {
  
  public:
   
    static IsoSurfacer* New();
    vtkTypeMacro(IsoSurfacer, vtkObject);
    
    vtkSetMacro(Value, double);
    vtkSetMacro(Input, vtkUnstructuredGrid *);
    vtkSetMacro(Type, extractionType);
	vtkSetMacro(TetNeighbors, vector< vector<vtkIdType> >*);
	vtkSetMacro(Index, TetIndex*);
    
    vtkGetMacro(Output, vtkPolyData *);
   
    void Update();
  
  protected:
    
    IsoSurfacer();
    ~IsoSurfacer();
	
    inline vector<double> ComputeEdgeIntersection(const pair<vtkIdType, vtkIdType> &edge) const{
    //QUESTION 8 FAIT 
      vector<double> p(3); //new point
	  
	  //Get value of edge ends 
	  double EdgeValue1 = Input->GetPointData()->GetScalars()->GetComponent(edge.first, 0);
	  double EdgeValue2 = Input->GetPointData()->GetScalars()->GetComponent(edge.second, 0);
	  //Get coordinates of edge ends
	  double *EdgePoint1 = new double[3];
	  double *EdgePoint2 = new double[3];
	 
	  Input->GetPoint(edge.first,EdgePoint1);
	  Input->GetPoint(edge.second,EdgePoint2);

	  //Compute the coordinate of the intersection 
	  double t = (Value - EdgeValue2) / (EdgeValue1 - EdgeValue2);
	  p[0] = EdgePoint1[0] * t + (1 - t)*EdgePoint2[0];
	  p[1] = EdgePoint1[1] * t + (1 - t)*EdgePoint2[1];
	  p[2] = EdgePoint1[2] * t + (1 - t)*EdgePoint2[2];
      return p;
    };
    
    int ComputePartialIntersection(const int &tetId);
      
    int ComputeSimpleIntersection(vtkCell *tet);
   
    int FastExtraction(); 
    
    inline bool IsCellOnLevelSet(vtkCell *cell) const{
		//cell type
		int nbPoints = cell->GetNumberOfPoints();
		
		//ids
		vtkIdList* ids = cell->GetPointIds();

		double min = Input->GetPointData()->GetScalars()->GetComponent(ids->GetId(0),0);
		double max = Input->GetPointData()->GetScalars()->GetComponent(ids->GetId(0),0);

		for (int i = 1 ; i < nbPoints; i++)
		{
			double val = Input->GetPointData()->GetScalars()->GetComponent(ids->GetId(i),0);
			if (val < min) min = val;
			else if (val > max) max = val;
		}
		
		return (Value >= min && Value <= max);
    };

    int ReOrderTetEdges(vector<pair<vtkIdType, vtkIdType> > &edgeList) const;
    
    int SimpleExtraction();
    
    int StandardExtraction();
    
    // variables for get/set macros
    vtkUnstructuredGrid               *Input;
    vtkPolyData                       *Output;
    extractionType                    Type;
    double                            Value;
	vector< vector<vtkIdType> >		  *TetNeighbors;

	vector< vector<EdgeIntersection*> >  IntersectedEdges;

	TetIndex*						Index; 
    
    // internal variables
    vtkPoints                         *pointSet_;
    vtkCellArray                      *cellArray_;
    vtkDoubleArray                    *fakeScalars_;
    
    // speed up
    vtkDataArray                      *scalarField_;
    
  private:
    // not implemented
    
    IsoSurfacer(const IsoSurfacer&);
    
    void operator=(const IsoSurfacer &);
};

#endif // ISOSURFACER_H
