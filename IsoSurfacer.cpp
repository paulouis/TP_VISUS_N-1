/*
IsoSurfacer.cpp: Isosurface computation class.
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


#include  "IsoSurfacer.h"

vtkStandardNewMacro(IsoSurfacer);

IsoSurfacer::IsoSurfacer(){
	Input = NULL;
	Output = NULL;
	pointSet_ = NULL;
	cellArray_ = NULL;
	fakeScalars_ = NULL;
	Type = SIMPLE;
}

IsoSurfacer::~IsoSurfacer(){

	if(Output)
		Output->Delete();
	if(pointSet_)
		pointSet_->Delete();
	if(cellArray_)
		cellArray_->Delete();
	if(fakeScalars_)
		fakeScalars_->Delete();
}

int IsoSurfacer::ComputePartialIntersection(const int &tetId){
	//QUESTION 16
	/*
	vtkIdList * createdPts = vtkIdList::New();
	vector<pair <vtkIdType,vtkIdType> >  tetEdges; 

	//compute neighbors of tetId and store them in the neighbors variable
	vector<vtkIdType>& neighbors = TetNeighbors->at(tetId);
	...


	//as in ComputeSimpleIntersection, store the edges intersected by the level set in tetEdges
	...

	
	//re-order the edges
	...


	for (int i = 0; i < tetEdges.size() ; i++)
	{
		//edge i
		bool computedIntersection = false; //computedIntersection is true if the intersection of this edge has been already computed

		//looping on the neighbors of tetId to see whether the edge intersection has already been computed 
		int j = 0;
		while (j < voisins.size() && !deja_traite)
		{
			//only neighboors intersected with the level-set and with an id inferior to tetId are good candidates 
			if (...)
			{
					// for each good candidate, we compare his already computed intersected edges to the current edge i
					int k = 0;
					while (k < edgesIntersected[voisins[j]].size() && !deja_traite)
					{
						EdgeIntersection* edgeintersection = edgesIntersected[voisins[j]][k];
						//if the edge vertex ids are the same
						if ( ...)
						{
							//the intersection is already computed
							computedIntersection = true;
							.....
						}
						k++;
					}	
			}
			j++;
		}

		//if intersection not already computed
		if (!deja_traite)
		{
			//computation of the intersection
			....


			//creation of a new EdgeIntersection and storing it in edgesIntersected
			...
		}	
	}

	//insert the created ids in the output surface
	...


	createdPts->Delete();*/

	return 0;
}

int IsoSurfacer::ComputeSimpleIntersection(vtkCell *tet){
	//QUESTION 9
	/*
	vtkIdList* createdPts = vtkIdList::New();
	vector< pair<vtkIdType,vtkIdType> >  tetEdges; 

	//In tetEdges, store the edges intersected by the level set
	for (int i = 0; i < tet->GetNumberOfEdges() ; i++)
	{
		


	}


	//for each edge of tetEdges, compute edge intersection and add the new vertex in the createdPts list
	for (int i = 0; i < tetEdges.size() ; i++)
	{



	}



	//insert the created ids in the output surface
	


	createdPts->Delete();

	*/

	return 0;
}

int IsoSurfacer::FastExtraction(){
	//QUESTION 22
	return 0;
}

int IsoSurfacer::ReOrderTetEdges(vector<pair<vtkIdType, vtkIdType> > &edgeList) const{
	int size = edgeList.size();

	pair<vtkIdType, vtkIdType> temp = edgeList[0];
	for (int i = 1; i < size-1; i++)
	{
		if (edgeList[i].first == temp.first || edgeList[i].second == temp.first || edgeList[i].first == temp.second || edgeList[i].second == temp.second)
		{
			temp = edgeList[i];
		} else 
		{
			temp = edgeList[i+1];
			edgeList[i+1] = edgeList[i];
			edgeList[i] = temp;
			break;
		}
	}

	temp = edgeList[size-1];
	for (int i = size-2; i > 0; i--)
	{
		if (edgeList[i].first == temp.first || edgeList[i].second == temp.first || edgeList[i].first == temp.second || edgeList[i].second == temp.second)
		{
			temp = edgeList[i];
		} else 
		{
			temp = edgeList[i-1];
			edgeList[i-1] = edgeList[i];
			edgeList[i] = temp;
			break;
		}
	}

	return 0;
}

int IsoSurfacer::SimpleExtraction(){
	//QUESTION 10
	//loop on all the tetrahedrons of the mesh

	// if the tetrahedron is on the level set, compute the intersection 

	return 0;
}

int IsoSurfacer::StandardExtraction(){
	//QUESTION 17


	return 0;
}

void IsoSurfacer::Update(){

	if(!Input){
		cerr << "[IsoSurfacer] No input defined..." << endl;
		return;
	}


	if(pointSet_)
		pointSet_->Delete();
	pointSet_ = vtkPoints::New();


	if(cellArray_)
		cellArray_->Delete();
	cellArray_ = vtkCellArray::New();

	if(Output)
		Output->Delete();

	if(!fakeScalars_)
		fakeScalars_ = vtkDoubleArray::New();

	Output = vtkPolyData::New();
	Output->SetPoints(pointSet_);
	Output->SetPolys(cellArray_);
	Output->GetPointData()->SetScalars(fakeScalars_);

	scalarField_ = Input->GetPointData()->GetScalars();

	DebugMemory memory;
	DebugTimer  timer;

	switch(Type){

	case SIMPLE:
		cout << "[IsoSurfacer] Using simple implementation..." << endl;
		SimpleExtraction();
		break;

	case STANDARD:
		cout << "[IsoSurfacer] Using standard implementation..." << endl;
		StandardExtraction();
		break;

	case FAST:
		cout << "[IsoSurfacer] Using fast implementation..." << endl;
		FastExtraction();
		break;
	}

	cout << setprecision(4);

	cout << "[IsoSurfacer] IsoSurface extracted ("
		<< Output->GetNumberOfPoints() << " vertices, "
		<< Output->GetNumberOfCells() << " faces)." << endl;
	cout << "[IsoSurfacer] Extraction performed in "
		<< timer.getElapsedTime() << " s. (memory usage: "
		<< memory.getInstantUsage() << " MB)." << endl;

	// add a scalar value to the isosurface to make sure it gets colored by the
	// rest of the pipeline
	fakeScalars_->SetNumberOfTuples(Output->GetNumberOfPoints());
	for(int i = 0; i < Output->GetNumberOfPoints(); i++)
		fakeScalars_->SetComponent(i, 0, Value);
}

