##############################################################################
# this file is parsed when FIND_PACKAGE is called with version argument
#
# @author Jan Engels, Desy IT
##############################################################################


SET( ${PACKAGE_FIND_NAME}_VERSION_MAJOR 0 )
SET( ${PACKAGE_FIND_NAME}_VERSION_MINOR 1 )
SET( ${PACKAGE_FIND_NAME}_VERSION_PATCH 0 )


# only do work if FIND_PACKAGE called with a version argument
IF( DEFINED ${PACKAGE_FIND_NAME}_FIND_VERSION )

    SET( _installed_version ${${PACKAGE_FIND_NAME}_VERSION_MAJOR}.${${PACKAGE_FIND_NAME}_VERSION_MINOR}.${${PACKAGE_FIND_NAME}_VERSION_PATCH} )
    SET( _searching_version ${${PACKAGE_FIND_NAME}_FIND_VERSION} )

    # these variables are evaluated internally by the cmake command FIND_PACKAGE to mark this
    # package as suitable or not depending on the required version
    SET( PACKAGE_VERSION_EXACT FALSE )
    SET( PACKAGE_VERSION_COMPATIBLE FALSE )
    SET( PACKAGE_VERSION_UNSUITABLE TRUE )

    IF( _installed_version VERSION_EQUAL _searching_version ) # if version matches EXACTLY

        IF( NOT ${PACKAGE_FIND_NAME}_FIND_QUIETLY )
            MESSAGE( STATUS "Check for ${PACKAGE_FIND_NAME}: exact version found: ${_installed_version}" )
        ENDIF()

        SET( PACKAGE_VERSION_EXACT TRUE )
        SET( PACKAGE_VERSION_UNSUITABLE FALSE )
        #SET( PACKAGE_VERSION_COMPATIBLE TRUE ) # FIXME also set COMPATIBLE if version matches EXACTLY ?!

    ELSE() # if version does not match EXACTLY, check if it is compatible/suitable

        IF( ${PACKAGE_FIND_NAME}_FIND_VERSION_EXACT ) # user required version to match EXACTLY and it failed!

            IF( NOT ${PACKAGE_FIND_NAME}_FIND_QUIETLY )
                MESSAGE( STATUS "Check for ${PACKAGE_FIND_NAME}: version found: ${_installed_version}" )
                MESSAGE( STATUS "Check for ${PACKAGE_FIND_NAME}: could not find exact version" )
            ENDIF()

            SET( PACKAGE_VERSION_UNSUITABLE TRUE )

        ELSE() # user did not require an EXACT match, let's check if version is compatible...

            # --- MINIMUM_REQUIRED_VERSION --------------------------------------------------------
            # this method behaves the same as with CMAKE_MINIMUM_REQUIRED, i.e. if user asks for
            # version 1.2.5 then any version >= 1.2.5 is suitable/compatible
            IF( _searching_version VERSION_LESS _installed_version )
                SET( PACKAGE_VERSION_COMPATIBLE TRUE )
            ENDIF()
            # -------------------------------------------------------------------------------------

        ENDIF()

    ENDIF()

    IF( PACKAGE_VERSION_COMPATIBLE )
        SET( PACKAGE_VERSION_UNSUITABLE FALSE )
        IF( NOT ${PACKAGE_FIND_NAME}_FIND_QUIETLY )
            MESSAGE( STATUS "Check for ${PACKAGE_FIND_NAME}: compatible version found: ${_installed_version}" )
        ENDIF()
    ENDIF()

    IF( PACKAGE_VERSION_UNSUITABLE )
        IF( NOT ${PACKAGE_FIND_NAME}_FIND_QUIETLY )
            #MESSAGE( STATUS "Check for ${PACKAGE_FIND_NAME}: unsuitable version found: ${_installed_version}" )
            MESSAGE( STATUS "Check for ${PACKAGE_FIND_NAME}: minimum required version not found" )
        ENDIF()
    ENDIF()

ENDIF()

