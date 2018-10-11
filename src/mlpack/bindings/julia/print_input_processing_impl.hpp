/**
 * @file print_input_processing_impl.hpp
 * @author Ryan Curtin
 *
 * Print Julia code to handle input arguments.
 */
#ifndef MLPACK_BINDINGS_JULIA_PRINT_INPUT_PROCESSING_IMPL_HPP
#define MLPACK_BINDINGS_JULIA_PRINT_INPUT_PROCESSING_IMPL_HPP

#include "strip_type.hpp"
#include "get_julia_type.hpp"

namespace mlpack {
namespace bindings {
namespace julia {

/**
 * Print the input processing (basically calling CLI::GetParam<>()) for a
 * non-serializable type.
 */
template<typename T>
void PrintInputProcessing(
    const util::ParamData& d,
    const typename std::enable_if<!data::HasSerialize<T>::value>::type*)
{
  // Here we can just call CLISetParam() directly; we don't need a separate
  // overload.
  if (d.required)
  {
    // This gives us code like the following:
    //
    // CLISetParam("<param_name>", convert(<type>, <paramName>))
    std::cout << "  CLISetParam(\"" << d.name << "\", convert("
        << GetJuliaType<T>() << ", " << d.name << "))" << std::endl;
  }
  else
  {
    // This gives us code like the following:
    //
    // if <param_name> !== nothing
    //   CLISetParam("<param_name>", convert(<type>, <param_name>))
    // end
    std::cout << "  if " << d.name << " !== nothing" << std::endl;
    std::cout << "    CLISetParam(\"" << d.name << "\", convert("
        << GetJuliaType<T>() << ", " << d.name << "))" << std::endl;
    std::cout << "  end" << std::endl;
  }
}

/**
 * Print the input processing for an Armadillo type.
 */
template<typename T>
void PrintInputProcessing(
    const util::ParamData& d,
    const typename std::enable_if<arma::is_arma_type<T>::value>::type* )
{
  // If the argument is not required, then we have to encase the code in an if.
  size_t extraIndent = 0;
  if (!d.required)
  {
    std::cout << "  if " << d.name << " !== nothing" << std::endl;
    extraIndent = 2;
  }

  // For an Armadillo type, we have to call a different overload for columns and
  // rows than for regular matrices.
  std::string uChar = (std::is_same<typename T::elem_type, size_t>::value) ?
      "U" : "";
  std::string indent(extraIndent + 2, ' ');
  std::string matTypeModifier = "";
  if (T::is_row)
    matTypeModifier = "Row";
  else if (T::is_col)
    matTypeModifier = "Col";

  // Now print the CLISetParam call.
  std::cout << indent << "CLISetParam" << uChar << matTypeModifier << "(\""
      << d.name << "\", convert(" << GetJuliaType<T>() << ", " << d.name << "))"
      << std::endl;

  if (!d.required)
  {
    std::cout << "  end" << std::endl;
  }
}

/**
 * Print the input processing for a serializable type.
 */
template<typename T>
void PrintInputProcessing(
    const util::ParamData& d,
    const typename std::enable_if<!arma::is_arma_type<T>::value>::type*,
    const typename std::enable_if<data::HasSerialize<T>::value>::type*)
{
  // If the argument is not required, then we have to encase the code in an if.
  size_t extraIndent = 0;
  if (!d.required)
  {
    std::cout << "  if " << d.name << " !== nothing" << std::endl;
    extraIndent = 2;
  }

  std::string indent(extraIndent + 2, ' ');
  std::string type = StripType(d.cppType);
  std::cout << indent << "CLISetParam" << type << "Ptr(\"" << d.name
      << "\", convert(" << GetJuliaType<T>() << ", " << d.name << "))"
      << std::endl;

  if (!d.required)
  {
    std::cout << "  end" << std::endl;
  }
}

} // namespace julia
} // namespace bindings
} // namespace mlpack

#endif
