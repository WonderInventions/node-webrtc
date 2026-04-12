@ECHO OFF
SET EL=0

ECHO Add depot_tools to PATH
set PATH=%DEPOT_TOOLS%;%PATH%
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

ECHO ninja
IF /I "%USE_CUSTOM_LIBCXX%"=="true" (
  call autoninja webrtc create_peerconnection_factory libc++
) ELSE (
  call autoninja webrtc create_peerconnection_factory
)
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

GOTO DONE

:ERROR
ECHO ERRORLEVEL^: %ERRORLEVEL%
SET EL=%ERRORLEVEL%

:DONE

EXIT /b %EL%
