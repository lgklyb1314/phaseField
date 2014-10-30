//Mechanics (infinitesimal strain) implementation
//general headers
#include "../../include/dealIIheaders.h"

//Mechanics problem headers
#include "parameters.h"
#include "../../src/mechanics.h"


//Mark boundaries for applying Dirichlet BC's
template <int dim>
void MechanicsProblem<dim>::mark_boundaries(){
  typename DoFHandler<dim>::active_cell_iterator cell = dof_handler.begin_active(), endc = dof_handler.end();
  //All boundaries are by marked with flag '0' by default. 
  //To pick specific boundaries, one needs to mark them 
  //with integer flags and use those flags in apply_dirichlet_conditons()
  for (;cell!=endc; ++cell){
    for (unsigned int f=0; f<GeometryInfo<dim>::faces_per_cell; ++f){
      if (cell->face(f)->at_boundary()){
	const Point<dim> face_center = cell->face(f)->center();
	if (face_center[0]==0.0){
	  cell->face(f)->set_boundary_indicator (1); //boundary at X=0.0 marked with flag '1'
	}
	else if (face_center[0]==spanX){
	  cell->face(f)->set_boundary_indicator (2); //boundary at X=spanX marked with flag '2'
	}
      }
    }
  }
}

//Set Dirichlet BC values 
template <int dim>
class BCFunction : public Function<dim>{
  public:
  BCFunction(): Function<dim> (dim){}
  void vector_value (const Point<dim>   &p, Vector<double>   &values) const{
    Assert (values.size() == dim, ExcDimensionMismatch (values.size(), dim));    
    values[0]=spanX/100.0; // displacement along X-Direction
  }
};

//Apply Dirchlet BC function
template <int dim>
void MechanicsProblem<dim>::apply_dirichlet_bcs (){
  //Set u=0 at X=0.0
  VectorTools::interpolate_boundary_values (dof_handler, 1, ZeroFunction<dim>(dim), constraints);
  //Set u[0]=spanX/100.0 at X=spanX
  std::vector<bool> xyzFlags (dim, false);
  xyzFlags[0]=true; //to apply dirichlet BC only along ux
  VectorTools::interpolate_boundary_values (dof_handler, 2, BCFunction<dim>(), constraints, xyzFlags);
}


//main
int main (int argc, char **argv)
{
  Utilities::System::MPI_InitFinalize mpi_initialization(argc, argv,numbers::invalid_unsigned_int);
  try
    {
      deallog.depth_console(0);
      MechanicsProblem<problemDIM> problem;
      problem.run ();
    }
  catch (std::exception &exc)
    {
      std::cerr << std::endl << std::endl
                << "----------------------------------------------------"
                << std::endl;
      std::cerr << "Exception on processing: " << std::endl
                << exc.what() << std::endl
                << "Aborting!" << std::endl
                << "----------------------------------------------------"
                << std::endl;
      return 1;
    }
  catch (...)
    {
      std::cerr << std::endl << std::endl
                << "----------------------------------------------------"
                << std::endl;
      std::cerr << "Unknown exception!" << std::endl
                << "Aborting!" << std::endl
                << "----------------------------------------------------"
                << std::endl;
      return 1;
    }
  
  return 0;
}