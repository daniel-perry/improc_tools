# The MIT License
#
# Copyright (c) University of Utah.
#
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
# 

# jc -- For more information building and packaging with CMake and Qt, see the
# following URL:  http://www.cmake.org/Wiki/BundleUtilitiesExample
# See also the following thread:
#  http://www.mail-archive.com/paraview@paraview.org/msg05234.html
PROJECT( improc_tools )
cmake_minimum_required(VERSION 2.8)  

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/Modules/")

#Match everything when analyzing dependencies
INCLUDE_REGULAR_EXPRESSION("^.*$")
OPTION (BUILD_SHARED_LIBS "Build shared libraries" OFF)

IF (WIN32 AND MSVC)
  ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF (WIN32 AND MSVC)

FIND_PACKAGE(ITK REQUIRED)
IF (ITK_FOUND)
  INCLUDE (${USE_ITK_FILE})
ELSE(ITK_FOUND)
  MESSAGE(STATUS "WARNING: ITK not found")
ENDIF(ITK_FOUND)

#FIND_PACKAGE(Silo REQUIRED)
#IF(Silo_FOUND)
#  INCLUDE_DIRECTORIES(${Silo_INCLUDE_DIR})
#  LINK_DIRECTORIES(${Silo_LIBRARY_DIR})
#ELSE(Silo_FOUND)
#  MESSAGE(STATUS "WARNING: Silo not found")
#ENDIF(Silo_FOUND)


#FIND_PACKAGE(VTK REQUIRED)
#IF (VTK_FOUND)
# INCLUDE( ${USE_VTK_FILE} )
#ENDIF(VTK_FOUND)

#find_package(OpenCV REQUIRED)

FIND_PACKAGE(Boost REQUIRED)
IF(BOOST_FOUND)
  INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${Boost_LIBRARY_DIRS} )
ENDIF(BOOST_FOUND)


SET(CMAKE_INCLUDE_CURRENT_DIR ON)

##########################################################################
# silo
##########################################################################

#SET( silo_SRCS
#     silo.cc
#)
#
##SET ( silo_HDRS
##     silo.h
##)
#
#
#ADD_EXECUTABLE( silotest
#                ${silo_SRCS}
##                ${silo_HDRS}
#				      )
#
##TARGET_LINK_LIBRARIES( silo ITKAlgorithms
##                        ITKBasicFilters 
##                        ITKIO ITKNumerics ITKCommon 
##                     ) 
#TARGET_LINK_LIBRARIES( silotest
#                       silo
#                       #${ITK_LIBRARIES}
#                     ) 
#


##########################################################################
# nrrd2silo
##########################################################################

#SET( nrrd2silo_SRCS
#     nrrd2silo.cc
#)
#
##SET ( nrrd2silo_HDRS
##     nrrd2silo.h
##)
#
#
#ADD_EXECUTABLE( nrrd2silo 
#                ${nrrd2silo_SRCS}
##                ${nrrd2silo_HDRS}
#				      )
#
#TARGET_LINK_LIBRARIES( nrrd2silo 
#                        silo
#                        #ITKAlgorithms
#                        #ITKBasicFilters 
#                        #ITKIONRRD 
#                        #ITKNrrdIO
#                        #ITKNumerics 
#                        #ITKCommon 
#                        ${ITK_LIBRARIES}
#                     ) 
#
##########################################################################
# Staple
##########################################################################

SET( STAPLE_SRCS
     staple.cc
)

SET ( STAPLE_HDRS
     staple.h
)


ADD_EXECUTABLE( staple 
                ${STAPLE_SRCS}
                ${STAPLE_HDRS}
				      )

#TARGET_LINK_LIBRARIES( staple ITKAlgorithms
#                        ITKBasicFilters 
#                        ITKIO ITKNumerics ITKCommon 
#                     ) 
TARGET_LINK_LIBRARIES( staple 
                       ${ITK_LIBRARIES}
                     ) 


##########################################################################
# Dice Similarity Coefficient (DSC)
##########################################################################

SET( dice_SRCS
     dice.cc
)

#SET ( dice_HDRS
#     dice.h
#)

ADD_EXECUTABLE( dice 
                ${dice_SRCS}
                #${dice_HDRS}
				      )

#TARGET_LINK_LIBRARIES( dice ITKAlgorithms
#                        ITKBasicFilters 
#                        ITKIO ITKNumerics ITKCommon 
#                     ) 
TARGET_LINK_LIBRARIES( dice 
                       ${ITK_LIBRARIES}
                     ) 


##########################################################################
# XOR Overlap 
##########################################################################

SET( xoroverlap_SRCS
     xoroverlap.cc
)

#SET ( xoroverlap_HDRS
#     dice.h
#)

ADD_EXECUTABLE( xoroverlap 
                ${xoroverlap_SRCS}
                #${xoroverlap_HDRS}
				      )

#TARGET_LINK_LIBRARIES( xoroverlap ITKAlgorithms
#                        ITKBasicFilters 
#                        ITKIO ITKNumerics ITKCommon 
#                     ) 
TARGET_LINK_LIBRARIES( xoroverlap 
                       ${ITK_LIBRARIES}
                     ) 


 
##########################################################################
# compress
##########################################################################

SET( compress_SRCS
     compress.cc
)


ADD_EXECUTABLE( compress
                ${compress_SRCS}
				      )

#TARGET_LINK_LIBRARIES( compress ITKAlgorithms
#                        ITKBasicFilters 
#                        ITKIO ITKNumerics ITKCommon 
#                     ) 
TARGET_LINK_LIBRARIES( compress 
                       ${ITK_LIBRARIES}
                     ) 


##########################################################################
# data_to_mask
##########################################################################

SET( DATA_TO_MASK_SRCS
     data_to_mask.cc
)


ADD_EXECUTABLE( data_to_mask
                ${DATA_TO_MASK_SRCS}
				      )

#TARGET_LINK_LIBRARIES( data_to_mask ITKAlgorithms
#                        ITKBasicFilters 
#                        ITKIO ITKNumerics ITKCommon
TARGET_LINK_LIBRARIES( data_to_mask 
                       ${ITK_LIBRARIES}
                     ) 

##########################################################################
# threshold
##########################################################################

SET( THRESHOLD_SRCS
     threshold.cc
)


ADD_EXECUTABLE( thresholdimage
                ${THRESHOLD_SRCS}
				      )

#TARGET_LINK_LIBRARIES( thresholdimage ITKAlgorithms
#                        ITKBasicFilters 
#                        ITKIO ITKNumerics ITKCommon 
TARGET_LINK_LIBRARIES( thresholdimage 
                       ${ITK_LIBRARIES}
                     ) 

##########################################################################
# logical
##########################################################################

SET( logical_SRCS
     logical.cc
)


ADD_EXECUTABLE( logicalimage
                ${logical_SRCS}
				      )

#TARGET_LINK_LIBRARIES( logicalimage ITKAlgorithms
#                        ITKBasicFilters 
#                        ITKIO ITKNumerics ITKCommon 
TARGET_LINK_LIBRARIES( logicalimage 
                       ${ITK_LIBRARIES}
                     ) 


