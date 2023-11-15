#ifndef __PTZ_CONTROL_H__
#define __PTZ_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ptz_control_protocol.h"

#define PTZ_USER_ID 0xff11
#define PTZ_ADDR_GUIDANCE_MODE_ANGLE 0
#define PTZ_ADDR_GUIDANCE_MODE_GEOGRAPHY   1
#define PTZ_PHOTOELECTRICITY_NO 1
#define PTZ_SYS_NO              1

//-----------------------ptz -> agx-----------------------
//0x01
#pragma pack(1)
typedef struct ptz_msg_status {
    uint32_t photoelectricity_no;
    uint64_t photoelectricity_status_timesmap;
    uint32_t work_status;
    uint32_t work_mode;
    uint64_t bitcode;
    char reserver[4];
}ptz_msg_status_t;
#pragma pack()

//0x0b
#pragma pack(1)
typedef struct ptz_msg_target_info {
    uint32_t target_no;
    uint32_t target_type;
    uint32_t similarty;
    uint32_t width;
    uint32_t height;
    uint32_t phy_width;
    uint32_t phy_height;
    uint8_t motion_drirection;
    uint8_t  ai_mode;
    uint16_t reserver;
    uint16_t horizatal_speed;
    uint16_t vertical_speed;
    uint16_t target_position_x;
    uint16_t target_position_y;
    double target_azimuth;
    double target_pitching;
    uint32_t target_distance;
}ptz_msg_target_info_t;
#pragma pack()

#pragma pack(1)
typedef struct ptz_msg_target_pac {
    uint32_t last_time;
    uint32_t target_num;
    uint32_t data_len;
    ptz_msg_target_info_t *targets;
}ptz_msg_target_pac_t;
#pragma pack()

//0x02
#pragma pack(1)
typedef struct ptz_msg_pitch_azimuth {
    uint32_t photoelectricity_no;
    uint64_t photoelectricity_status_timesmap;
    double horizatal_angle;
    double pitching_angle;
    double distance;
    double horizatal_angle_speed;
    double pitch_angle_speed;
    uint16_t target_height;
    uint8_t lens_magnification;
    uint8_t reserver;
}ptz_msg_pitch_azimuth_t;
#pragma pack()

//0x08
#pragma pack(1)
typedef struct ptz_msg_dev_target_info {
    uint32_t targt_no;
    uint32_t target_type;
    uint32_t similarity;
    uint32_t target_width;
    uint32_t target_height;
    double   target_azimuth;
    double   target_pitching_angle;
}ptz_msg_dev_target_info_t;

typedef struct ptz_msg_dev_status_search {
    uint32_t time;
    uint32_t target_num;
    uint32_t data_len;
    //data
}ptz_msg_dev_status_search_t;

typedef struct ptz_msg_dev_status_track {
    uint32_t time;
    uint16_t user_id;
    uint16_t target_status;
    //target info
    ptz_msg_dev_target_info_t target;
}ptz_msg_dev_status_track_t;

typedef struct ptz_msg_dev_ext_info {
    uint32_t photoelectricity_no;
    uint64_t sys_timesmap;
    uint32_t work_status;
    uint32_t data_len;    
}ptz_msg_dev_ext_info_t;
#pragma pack()

#pragma pack(1)
typedef struct ptz_msg_lense_ext_info{
    uint32_t photoelectricity_no;
    uint64_t cur_status_timesmap;
    uint32_t visable_horizatal_angle;
    uint32_t visable_vertical_angle;
    uint32_t thermal_horization_angle;
    uint32_t thermal_vertical_angle;
    uint32_t visable_zoom;
    uint32_t thermal_zoom;
    uint32_t visable_focal;
    uint32_t thermal_focal;
}ptz_msg_lense_ext_info_t;
#pragma pack()

#pragma pack(1)
typedef struct ptz_msg_sys_ext_info{
    uint32_t photoelectricity_no;
    uint64_t cur_status_timesmap;
    uint32_t laser_rangefinder_status;
    uint32_t laser_measuring_status;
    uint32_t laser_fill_state;
    uint32_t heater_condition;
    uint32_t thermal_imaging_status;
    uint32_t visible_focusing_mode;
    uint32_t thermal_focusing_mode;
    uint8_t power_status_ext_device;
    uint8_t reserver[31];
}ptz_msg_sys_ext_info_t;
#pragma pack()

//-----------------------agx -> ptz-----------------------
//0x03
#pragma pack(1)
typedef struct ptz_msgid_0x03_addr{
    uint32_t photoelectricity_no;
    uint32_t sys_no;
    uint64_t sys_timesmap;
    double   target_longitude;
    double   target_latitude;
    double   target_altitude;
    double   distance;
    double   horizatal_angle;
    double   pitch_angle;
    uint16_t user_id;
    uint8_t  guidance_mode;
    uint8_t  target_motion_direction;
}ptz_msgid_0x03_addr_t;
#pragma pack()

//0x0d
#pragma pack(1)
typedef struct ptz_msg_direction_ctrl {
    uint32_t photoelectricity_no;
    uint64_t sys_timesmap;
    uint32_t control_cmd;
    uint32_t motion_level;
    uint32_t motion_way;
    uint32_t reserver;
}ptz_msg_direction_ctrl_t;
#pragma pack()

#pragma pack(1)
typedef struct ptz_msg_addr_external {
    uint32_t photoelectricity_no;
    uint32_t sys_no;
    uint64_t sys_timesmap;
    double target_longitude;
    double target_altitude;
    double target_height;
    double distance;
    double   horizatal_angle;
    double   pitch_angle;
    uint16_t user_id;
    uint8_t  guidance_mode;
    uint8_t  target_motion_direction;
    uint32_t search_mode;
    uint32_t right_left_search_eara;
    uint32_t up_down_search_eara;
    uint32_t reserver;
}ptz_msg_addr_external_t;
#pragma pack()

#pragma pack(1)
typedef struct ptz_msg_ctrl_pack {
    uint32_t photoelectricity_no;
    uint32_t sys_no;
    uint64_t sys_timesmap;
    uint32_t ctrl_cmd;
    uint32_t focal;
    uint32_t zoom;
    uint32_t focus;
    uint32_t reserver;
}ptz_msg_ctrl_pack_t;
#pragma pack()

#pragma pack(1)
typedef struct ptz_msg_track {
    uint32_t photoelectricity_no;
    uint64_t sys_timesmap;
    uint32_t ctrl_cmd;
    uint32_t horizatal_search_begin;
    uint32_t horizatal_search_end;
    uint32_t pitching_search_begin;
    uint32_t pitching_search_end;
    uint32_t reserver;
}ptz_msg_track_t;
#pragma pack()

#pragma pack(1)
typedef struct ptz_msg_manual_lock_target {
    uint32_t photoelectricity_no;
    uint64_t sys_timesmap;
    uint32_t track_vedio_source;
    uint32_t target_cor_x;
    uint32_t targegt_cor_y;
    uint32_t reserver;
}ptz_msg_manual_lock_target_t;
#pragma pack()

#pragma pack(1)
typedef struct ptz_msg_scan_ext {
    uint32_t photoelectricity_no;
    uint64_t sys_timesmap;
    uint32_t ctrl_cmd;
    uint32_t horizatal_search_begin;
    uint32_t horizatal_search_end;
    uint32_t pitching_search_begin;
    uint32_t pitching_search_end;
    uint32_t scan_speed;
    uint32_t reserver;
}ptz_msg_scan_ext_t;
#pragma pack()

#pragma pack(1)
typedef struct ptz_msg_peripheral_ctrl {
    uint32_t photoelectricity_no;
    uint64_t sys_timesmap;
    uint32_t ctrl_cmd;
    uint32_t ctrl_param;
    uint32_t reserver;
}ptz_msg_peripheral_ctrl_t;
#pragma pack()

#pragma pack(1)
typedef struct ptz_msg_ai_ctrl {
    uint32_t photoelectricity_no;
    uint64_t sys_timesmap;
    uint32_t config_cmd;
    uint32_t param_len;
    char*   param;
}ptz_msg_ai_ctrl_t;
#pragma pack()

#pragma pack(1)
typedef struct ptz_msg_config_focal_mode {
    uint32_t photoelectricity_no;
    uint64_t sys_timesmap;
    uint32_t focal_mode;
    uint8_t channel;
    uint8_t reserver[3];
}ptz_msg_config_focal_mode_t;
#pragma pack()

#pragma pack(1)
typedef struct ptz_msg_config_track_param {
    uint32_t photoelectricity_no;
    uint64_t sys_timesmap;
    uint32_t enable_focal_auto;
    uint32_t focal_magnifying_limit;
    uint32_t focal_reduce_limit;
    uint8_t reserver[32];
}ptz_msg_config_track_param_t;
#pragma pack()

int send_ptz_control_addr(ptz_msgid_0x03_addr_t *addr_t);
int send_ptz_control_direction(ptz_msg_direction_ctrl_t *dir_ctrl);
int send_ptz_control_add_ext(ptz_msg_addr_external_t *addr_ext);
int send_ptz_control_pack(ptz_msg_ctrl_pack_t *ctrl_p);
int send_ptz_control_track(ptz_msg_track_t *track);
int send_ptz_control_manual_lock_target(ptz_msg_manual_lock_target_t *track);
int send_ptz_control_scan_ext(ptz_msg_scan_ext_t *scan);

#ifdef __cplusplus
}
#endif

#endif
