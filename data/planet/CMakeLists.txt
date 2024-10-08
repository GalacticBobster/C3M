
## set up all tests ##

file(GLOB _tests *.cpp)
foreach(_test ${_tests})
  foreach(build ${BUILD_TYPES})
    string(TOLOWER ${build} buildl)
    get_filename_component(_name ${_test} NAME_WE)

    add_executable(${_name}.${buildl} ${_test})

    set_target_properties(${_name}.${buildl}
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${EXECUTABLE_RELDIR}"
      COMPILE_FLAGS ${CMAKE_CXX_FLAGS_${build}}
      )

    target_include_directories(${_name}.${buildl}
      SYSTEM PRIVATE
      ${C3M_INCLUDE_DIR}
      ${EIGEN3_INCLUDE_DIR}
      ${CANTERA_INCLUDE_DIR}
      ${NETCDF_INCLUDES}
      ${HDF5_INCLUDE_DIRS}
      OpenMP::OpenMP_CXX
      ${ATHENAPP_INCLUDE_DIR}
      ${CMAKE_BINARY_DIR}
      )

    #target_link_directories(${_name}.${buildl}
    #  SYSTEM PRIVATE
    #  ${CANTERA_LIBRARY_DIR}
    #  )


    target_link_libraries(${_name}.${buildl}
      ${C3M_LIBRARY_${build}}
      ${CANTERA_LIBRARY}
      ${NETCDF_LIBRARIES}
      ${HDF5_LIBRARIES}
      ${CMAKE_DL_LIBS}
      stdc++fs
      OpenMP::OpenMP_CXX
      ${ATHENAPP_LIBRARY_${build}}
      python3.9
      )
  endforeach()
endforeach()

# model input files
file(GLOB _inputs 
  *.yaml
  *.txt)
foreach(_input ${_inputs})
  get_filename_component(_name ${_input} NAME)

  configure_file(${_input} ${CMAKE_BINARY_DIR}/${EXECUTABLE_RELDIR}/${_name} COPYONLY)
endforeach()
