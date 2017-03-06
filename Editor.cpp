/*
  Editor.cpp: Data-structures and processing.
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


#include  "Editor.h"

Editor::Editor(){

  inputMesh_ = NULL;
  meshReader_ = NULL;
  type_ = SIMPLE;
}

Editor::Editor(const Editor& other){

}

Editor::~Editor(){

#ifndef _WIN32
  if(meshReader_) meshReader_->Delete();

  for(unsigned int i = 0; i < vtkIsoSurfacers_.size(); i++){
    if(vtkIsoSurfacers_[i])
      vtkIsoSurfacers_[i]->Delete();
  }

  for(unsigned int i = 0; i < isoSurfacers_.size(); i++){
    if(isoSurfacers_[i])
      isoSurfacers_[i]->Delete();
  }
#endif
}

Editor& Editor::operator=(const Editor& other){
  return *this;
}

bool Editor::operator==(const Editor& other) const{

  ///TODO: return ...;
  return false;
}

int Editor::addIsoSurface(const double &isoValue, const bool &useVtk){

  if(useVtk){
    vtkContourGrid *isoSurfacer = extractIsoSurfaceVTK(isoValue);
    if(isoSurfacer){

      isoSurfaceList_.push_back(isoSurfacer->GetOutput());
      vtkIsoSurfacers_.push_back(isoSurfacer);
      isoSurfacers_.push_back(NULL);
    }
  }
  else{
    IsoSurfacer *isoSurfacer = extractIsoSurface(isoValue);

    if(isoSurfacer){
      isoSurfaceList_.push_back(isoSurfacer->GetOutput());
      isoSurfacers_.push_back(isoSurfacer);
      vtkIsoSurfacers_.push_back(NULL);
    }
  }

  isoValueList_.push_back(isoValue);

  return isoSurfaceList_.size() - 1;
}

int Editor::deleteIsoSurface(const int &isoSurfaceId){

  if((isoSurfaceId < 0)||(isoSurfaceId >= (int) isoSurfaceList_.size()))
    return -1;

  isoValueList_.erase(isoValueList_.begin() + isoSurfaceId);
  isoSurfaceList_.erase(isoSurfaceList_.begin() + isoSurfaceId);

  if(vtkIsoSurfacers_[isoSurfaceId]){
    // vtk was used
    vtkIsoSurfacers_[isoSurfaceId]->Delete();
    vtkIsoSurfacers_.erase(vtkIsoSurfacers_.begin() + isoSurfaceId);
    isoSurfacers_.erase(isoSurfacers_.begin() + isoSurfaceId);
  }
  else{
    isoSurfacers_[isoSurfaceId]->Delete();
    isoSurfacers_.erase(isoSurfacers_.begin() + isoSurfaceId);
    vtkIsoSurfacers_.erase(vtkIsoSurfacers_.begin() + isoSurfaceId);
  }

  cout << "[Editor] Isosurface #" << isoSurfaceId << " deleted." << endl;

  if(!isoSurfaceList_.size())
    return -1;

  if(isoSurfaceId >= (int) isoSurfaceList_.size())
    return isoSurfaceList_.size() - 1;

  return -1;
}

IsoSurfacer* Editor::extractIsoSurface(const double &isoValue){
	DebugTimer t;
	//QUESTION 6
	IsoSurfacer* isosurface = IsoSurfacer::New();

	isosurface->SetType(Editor::type_);
	//set input data and isovalue
	isosurface->SetInput(inputMesh_);
	isosurface->SetValue(isoValue);

	//QUESTION 15:set neighbors
	isosurface->SetTetNeighbors(&tetNeighbors_);

	//QUESTION 21: set index
	isosurface->SetIndex(&tetIndex_);

	//update call
	isosurface->Update();

	cout << "[Editor] Isosurface extracted in " << t.getElapsedTime()
		<< " s." <<  endl;

	return isosurface;
}

vtkContourGrid* Editor::extractIsoSurfaceVTK(const double &isoValue){
  DebugTimer t;

  //QUESTION 4
  vtkContourGrid* isosurface = vtkContourGrid::New();
  //set input data and isovalue
  isosurface->SetInputData(inputMesh_);
  isosurface->SetValue(1, isoValue);

  //update call
  isosurface->Update();

  cout << "[Editor] Isosurface extracted in " << t.getElapsedTime()
    << " s." << endl;

  return isosurface;
}

vtkPolyData* Editor::getIsoSurface(const int &isoSurfaceId) const{

  if((isoSurfaceId < 0)||(isoSurfaceId >= (int) isoSurfaceList_.size()))
    return NULL;

  return isoSurfaceList_[isoSurfaceId];
}

int Editor::loadInputMesh (const string &fileName){

  meshReader_ = vtkXMLUnstructuredGridReader::New();

  meshReader_->SetFileName(fileName.data());

  cout << "[Editor] Reading input mesh..." << endl;

  meshReader_->Update();

  inputMesh_ = meshReader_->GetOutput();

  // force the usage of the first array as scalar data
  inputMesh_->GetPointData()->SetScalars(
    inputMesh_->GetPointData()->GetArray(0));

  inputMesh_->BuildLinks();

  cout << "[Editor]   done! (read " << inputMesh_->GetNumberOfCells()
    << " cells)" << endl;
  
  //QUESTION 14
  //tetNeighbors_ initialisation
  vtkIdList *cell_id_list = vtkIdList::New(), *points = vtkIdList::New(), *points_tetra = vtkIdList::New();
  std::vector<vtkIdType> voisins;
  for (int i = 0; i < inputMesh_->GetNumberOfCells(); i++)
  {
	  inputMesh_->GetCellPoints(i, points_tetra);
	  voisins.clear();
	  for (int j = 0; j < 4; j++)
	  {
		  for (int a = j+1; a < 4; a++)
		  {
			  for (int b = a+1; b < 4; b++)
			  {
				  points->Reset();
				  points->InsertNextId(points_tetra->GetId(j));
				  points->InsertNextId(points_tetra->GetId(a));
				  points->InsertNextId(points_tetra->GetId(b));
				  inputMesh_->GetCellNeighbors(i, points, cell_id_list);
				  for (int l = 0; l < cell_id_list->GetNumberOfIds(); l++)
				  {
					  voisins.push_back(cell_id_list->GetId(l));
				  }
			  }
		  }
	  }
	  for (int a = 0; a < 4; a++)
	  {
		  for (int b = a + 1; b < 4; b++)
		  {
			  points->Reset();
			  points->InsertNextId(points_tetra->GetId(a));
			  points->InsertNextId(points_tetra->GetId(b));
			  inputMesh_->GetCellNeighbors(i, points, cell_id_list);
			  for (int l = 0; l < cell_id_list->GetNumberOfIds(); l++)
			  {
				  voisins.push_back(cell_id_list->GetId(l));
			  }
		  }
	  }
	  tetNeighbors_.push_back(voisins);
  }


  //QUESTION 20
  tetIndex_.SetResolution(10000);
  //tetIndex_ initialisation (interval table)
  for (int i = 0; i < inputMesh_->GetNumberOfCells(); i++)
  {

	  int min = inputMesh_->GetPointData()->GetScalars()->GetComponent(inputMesh_->GetCell(i)->GetPointId(0), 0);
	  int max = inputMesh_->GetPointData()->GetScalars()->GetComponent(inputMesh_->GetCell(i)->GetPointId(0), 0);
	  for (int j = 0; j< inputMesh_->GetCell(i)->GetNumberOfPoints();j++)
	  { 
		 vtkIdType Id = inputMesh_->GetCell(i)->GetPointId(j);
		 double value = inputMesh_->GetPointData()->GetScalars()->GetComponent(Id, 0);
			if (value < min)
			{
				min = value;

			}
			else if (value > max)
			{

				max = value;
			}
	  
	  }
	 
	  tetIndex_.AddTet(i, min, max);

  }
  


  return 0;
}

int Editor::moveIsoSurface(const int& isoSurfaceId, const double &shift,
  const bool &useVtk){

  if((isoSurfaceId < 0)||(isoSurfaceId >= (int) isoSurfaceList_.size()))
    return -1;

  double newIsoValue = isoValueList_[isoSurfaceId] + shift;

  cout << "[Editor] Moving isosurface #" << isoSurfaceId
    << " to " << newIsoValue << "..." << endl;

  // delete the previous computation
  if(isoSurfacers_[isoSurfaceId]){
    isoSurfacers_[isoSurfaceId]->Delete();
    isoSurfacers_[isoSurfaceId] = NULL;
  }
  if(vtkIsoSurfacers_[isoSurfaceId]){
    vtkIsoSurfacers_[isoSurfaceId]->Delete();
    vtkIsoSurfacers_[isoSurfaceId] = NULL;
  }

  // now update
  if(useVtk){
    vtkContourGrid *isoSurfacer = extractIsoSurfaceVTK(newIsoValue);

    isoSurfaceList_[isoSurfaceId] = isoSurfacer->GetOutput();
    vtkIsoSurfacers_[isoSurfaceId] = isoSurfacer;
  }
  else{
    IsoSurfacer *isoSurfacer = extractIsoSurface(newIsoValue);

    isoSurfaceList_[isoSurfaceId] = isoSurfacer->GetOutput();
    isoSurfacers_[isoSurfaceId] = isoSurfacer;
  }

  isoValueList_[isoSurfaceId] = newIsoValue;

  return 0;
}

