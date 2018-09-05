
// requests & replies 
#define MSP_API_VERSION            1
#define MSP_FC_VARIANT             2
#define MSP_FC_VERSION             3
#define MSP_BOARD_INFO             4
#define MSP_BUILD_INFO             5
#define MSP_CALIBRATION_DATA      14
#define MSP_FEATURE               36
#define MSP_BOARD_ALIGNMENT       38
#define MSP_CURRENT_METER_CONFIG  40
#define MSP_RX_CONFIG             44
#define MSP_SONAR_ALTITUDE        58
#define MSP_ARMING_CONFIG         61
#define MSP_RX_MAP                64 // get channel map (also returns number of channels total)
#define MSP_LOOP_TIME             73 // FC cycle time i.e looptime parameter
#define MSP_STATUS               101
#define MSP_RAW_IMU              102
#define MSP_SERVO                103
#define MSP_MOTOR                104
#define MSP_RC                   105
#define MSP_RAW_GPS              106
#define MSP_COMP_GPS             107 // distance home, direction home
#define MSP_ATTITUDE             108
#define MSP_ALTITUDE             109
#define MSP_ANALOG               110
#define MSP_RC_TUNING            111 // rc rate, rc expo, rollpitch rate, yaw rate, dyn throttle PID
#define MSP_PID                  112 // P I D coeff
#define MSP_MISC                 114
#define MSP_SERVO_CONFIGURATIONS 120
#define MSP_NAV_STATUS           121 // navigation status
#define MSP_SENSOR_ALIGNMENT     126 // orientation of acc,gyro,mag
#define MSP_STATUS_EX            150
#define MSP_SENSOR_STATUS        151
#define MSP_BOXIDS               119
#define MSP_UID                  160 // Unique device ID
#define MSP_GPSSVINFO            164 // get Signal Strength (only U-Blox)
#define MSP_GPSSTATISTICS        166 // get GPS debugging data
#define MSP_SET_PID              202 // set P I D coeff


// commands
#define MSP_SET_HEAD             211 // define a new heading hold direction
#define MSP_SET_RAW_RC           200 // 8 rc chan
#define MSP_SET_RAW_GPS          201 // fix, numsat, lat, lon, alt, speed
#define MSP_SET_WP               209 // sets a given WP (WP#, lat, lon, alt, flags)


// bits of getActiveModes() return value 
#define MSP_MODE_ARM          0
#define MSP_MODE_ANGLE        1
#define MSP_MODE_HORIZON      2
#define MSP_MODE_NAVALTHOLD   3 /* cleanflight BARO */
#define MSP_MODE_MAG          4
#define MSP_MODE_HEADFREE     5
#define MSP_MODE_HEADADJ      6
#define MSP_MODE_CAMSTAB      7
#define MSP_MODE_NAVRTH       8 /* cleanflight GPSHOME */
#define MSP_MODE_NAVPOSHOLD   9 /* cleanflight GPSHOLD */
#define MSP_MODE_PASSTHRU    10
#define MSP_MODE_BEEPERON    11
#define MSP_MODE_LEDLOW      12
#define MSP_MODE_LLIGHTS     13
#define MSP_MODE_OSD         14
#define MSP_MODE_TELEMETRY   15
#define MSP_MODE_GTUNE       16
#define MSP_MODE_SONAR       17
#define MSP_MODE_BLACKBOX    18
#define MSP_MODE_FAILSAFE    19
#define MSP_MODE_NAVWP       20 /* cleanflight AIRMODE */
#define MSP_MODE_AIRMODE     21 /* cleanflight DISABLE3DSWITCH */
#define MSP_MODE_HOMERESET   22 /* cleanflight FPVANGLEMIX */
#define MSP_MODE_GCSNAV      23 /* cleanflight BLACKBOXERASE */
#define MSP_MODE_HEADINGLOCK 24
#define MSP_MODE_SURFACE     25
#define MSP_MODE_FLAPERON    26
#define MSP_MODE_TURNASSIST  27
#define MSP_MODE_NAVLAUNCH   28
#define MSP_MODE_AUTOTRIM    29

