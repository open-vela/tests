ifneq ($(CONFIG_MEDIA_GRAPH_TEST)$(CONFIG_MEDIA_STABILITY_TEST),)
	CSRCS += ./media_test/media_graph/media_graph_test.c
endif
ifneq ($(CONFIG_MEDIA_CLIENT_TEST)$(CONFIG_MEDIA_SERVER_TEST),)
	CSRCS += ./media_test/media_rpc/media_rpc.c
endif

ifneq ($(CONFIG_MEDIA_GRAPH_TEST),)
	PROGNAME += media_graph_open
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_open.c
	PROGNAME += media_graph_stop
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_stop.c
	PROGNAME += media_graph_pause
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_pause.c
	CSRCS += ./media_test/media_policy/MediaPolicyTest.c
	PROGNAME += media_stream_switch
	MAINSRC += $(CURDIR)/media_test/media_policy/MediaStreamSwitch.c
	PROGNAME += media_policy_setget
	MAINSRC += $(CURDIR)/media_test/media_policy/MediaSetGet.c
	PROGNAME += media_policy_indecrease
	MAINSRC += $(CURDIR)/media_test/media_policy/MediaInDecrease.c
	PROGNAME += media_policy_modeChange
	MAINSRC += $(CURDIR)/media_test/media_policy/MediaModeChange.c
	PROGNAME += media_policy_switch
	MAINSRC += $(CURDIR)/media_test/media_policy/MediaSwitch.c
	PROGNAME += media_policy_set_volume
	MAINSRC += $(CURDIR)/media_test/media_policy/MediaVolumeAdjust.c
	PROGNAME += media_loop_open
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_loop_open.c
	PROGNAME += media_graph_start
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_start.c
	PROGNAME += media_graph_single_volume
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_set_volume_test.c
	PROGNAME += media_graph_mul_volume
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_mul_set_volume_test.c
	PROGNAME += media_policy_mul_volume
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_mul_set_stream_volume.c
	PROGNAME += media_policy_single_volume
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_set_stream_volume.c
	PROGNAME += media_graph_isplay_test
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_isplay_test.c
	PROGNAME += media_graph_duration_test
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_duration_test.c
	PROGNAME += media_graph_seek
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_seek_and_position.c
	PROGNAME += media_graph_loop_test
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_loop_test.c
	PROGNAME += media_graph_record_time
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_record_time.c
	PROGNAME += media_graph_reset
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_reset.c
	PROGNAME += media_loop_play
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_loop_play.c
	PROGNAME += media_loop_take_picture
	MAINSRC += $(CURDIR)/media_test/media_graph/media_graph_loop_take_picture.c
	ifeq ($(CONFIG_LIBUV_EXTENSION),y)
		PROGNAME += media_uv_start
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_start.c
		PROGNAME += media_uv_stop
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_stop.c
		PROGNAME += media_uv_reset
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_reset.c
		PROGNAME += media_uv_position
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_position.c
		PROGNAME += media_uv_pause
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_pause.c
		PROGNAME += media_uv_duration
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_duration.c
		PROGNAME += media_uv_isplaying
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_isplaying.c
		PROGNAME += media_uv_policy_volume
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_policy_volume.c
		PROGNAME += media_uv_policy_dein_volume
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_policy_dein_volume.c
		PROGNAME += media_uv_record_time
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_record_time.c
		PROGNAME += media_uv_stab_play
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_stab_play.c
		PROGNAME += media_uv_stab_volume
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_stab_volume.c
		PROGNAME += media_uv_stab_pause
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_stab_pause.c
		PROGNAME += media_uv_loop_open
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_loop_open.c
		PROGNAME += media_uv_stab_conflict
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_stab_conflict.c
		PROGNAME += media_uv_stab_switch
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_stab_switch.c
		PROGNAME += media_uv_stab_seek
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_stab_seek.c
		PROGNAME += media_uv_stab_multi_seek
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_stab_multi_seek.c
		PROGNAME += media_uv_stab_multi_play
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_stab_multi_play.c
		PROGNAME += media_uv_set_loop
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_set_loop.c
		PROGNAME += media_uv_graph_volume
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_graph_volume.c
		PROGNAME += media_uv_seek_position
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_seek_position.c
		PROGNAME += media_uv_loop_play
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_loop_play.c
		PROGNAME += media_uv_timestamp
		MAINSRC += $(CURDIR)/media_test/media_uv/media_uv_timestamp.c
	endif
endif

ifneq ($(CONFIG_MEDIA_CLIENT_TEST),)
	PROGNAME += media_rpc_client_test01
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_client/media_client01.c
	PROGNAME += media_rpc_client_test02
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_client/media_client02.c
	PROGNAME += media_rpc_client_test03
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_client/media_client03.c
	PROGNAME += media_rpc_client_test04
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_client/media_client04.c
	PROGNAME += media_rpc_client_test05
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_client/media_client05.c
	PROGNAME += media_rpc_client_test06
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_client/media_client06.c
	PROGNAME += media_rpc_client_test07
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_client/media_client07.c
	PROGNAME += media_rpc_parcel_test01
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_parcel/media_parcel01.c
	PROGNAME += media_rpc_parcel_test02
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_parcel/media_parcel02.c
endif

ifneq ($(CONFIG_MEDIA_SERVER_TEST),)
	PROGNAME += media_rpc_server_t
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_server/media_server.c
	PROGNAME += media_rpc_server_test01
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_server/media_server01.c
	PROGNAME += media_rpc_server_test02
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_server/media_server02.c
	PROGNAME += media_rpc_server_test03
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_server/media_server03.c
	PROGNAME += media_rpc_server_test04
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_server/media_server04.c
	PROGNAME += media_rpc_server_test05
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_server/media_server05.c
	PROGNAME += media_rpc_server_test06
	MAINSRC += $(CURDIR)/media_test/media_rpc/rpc_server/media_server06.c
endif

ifneq ($(CONFIG_MEDIA_FOCUS_TEST),)
	CSRCS += ./media_test/media_focus/media_focus_test.c
	PROGNAME += media_focus_request_api_test01
	MAINSRC += $(CURDIR)/media_test/media_focus/media_focus_request_api_test01.c
	PROGNAME += media_focus_request_api_test02
	MAINSRC += $(CURDIR)/media_test/media_focus/media_focus_request_api_test02.c
	PROGNAME += media_focus_request_api_test03
	MAINSRC += $(CURDIR)/media_test/media_focus/media_focus_request_api_test03.c
	PROGNAME += media_focus_request_api_test04
	MAINSRC += $(CURDIR)/media_test/media_focus/media_focus_request_api_test04.c
	PROGNAME += media_focus_stack_test01
	MAINSRC += $(CURDIR)/media_test/media_focus/media_focus_stack_test01.c
	PROGNAME += media_focus_stack_test02
	MAINSRC += $(CURDIR)/media_test/media_focus/media_focus_stack_test02.c
	PROGNAME += media_focus_stack_test03
	MAINSRC += $(CURDIR)/media_test/media_focus/media_focus_stack_test03.c
	PROGNAME += media_focus_stack_test04
	MAINSRC += $(CURDIR)/media_test/media_focus/media_focus_stack_test04.c
	PROGNAME += media_focus_event_test01
	MAINSRC += $(CURDIR)/media_test/media_focus/media_focus_event_test01.c
	PROGNAME += media_focus_app_test01
	MAINSRC += $(CURDIR)/media_test/media_focus/media_focus_app_test01.c
	PROGNAME += media_focus_app_test02
	MAINSRC += $(CURDIR)/media_test/media_focus/media_focus_app_test02.c
	PROGNAME += media_focus_app_test03
	MAINSRC += $(CURDIR)/media_test/media_focus/media_focus_app_test03.c
	PROGNAME += media_focus_nomemory_test01
	MAINSRC += $(CURDIR)/media_test/media_focus/media_focus_nomemory_test01.c
	PROGNAME += media_focus_read_matrix_test01
	MAINSRC += $(CURDIR)/media_test/media_focus/media_focus_read_matrix_test01.c
endif

ifneq ($(CONFIG_MEDIA_STABILITY_TEST),)
	PROGNAME += media_stab_trans_phone
	MAINSRC += $(CURDIR)/media_test/media_stability/policy_trans_phone.c
	PROGNAME += media_stab_trans_blue
	MAINSRC += $(CURDIR)/media_test/media_stability/policy_trans_blue.c
	PROGNAME += media_stab_play_list
	MAINSRC += $(CURDIR)/media_test/media_stability/media_play_list.c
	PROGNAME += media_stab_play_pause
	MAINSRC += $(CURDIR)/media_test/media_stability/media_play_pause.c
	PROGNAME += mediatest_stab_switch
	MAINSRC += $(CURDIR)/media_test/media_stability/mediatest_switch.c
	PROGNAME += mediatest_stab_play_multi
	MAINSRC += $(CURDIR)/media_test/media_stability/mediatest_play_multi_pause.c
	PROGNAME += mediatest_stab_play_next
	MAINSRC += $(CURDIR)/media_test/media_stability/media_play_next.c
	PROGNAME += media_stab_set_volume
	MAINSRC += $(CURDIR)/media_test/media_stability/media_volume.c
	PROGNAME += media_stab_play_seek
	MAINSRC += $(CURDIR)/media_test/media_stability/media_play_seek.c
	PROGNAME += media_play_multi_seek
	MAINSRC += $(CURDIR)/media_test/media_stability/media_play_multi_seek.c
	ifeq ($(CONFIG_AUDIOUTILS_ALSA_LIB),y)
	PROGNAME += alsa_stab_play_list
	MAINSRC += $(CURDIR)/media_test/media_stability/media_alsa_play_list.c
	endif
endif

ifneq ($(CONFIG_MEDIATEST_TOOL),)
	PROGNAME += mediatest
	MAINSRC += $(CURDIR)/media_test/media_comp/media_player.c
endif

ifneq ($(CONFIG_MEDIATEST_PLAYER),)
	PROGNAME += mediatest_player
	MAINSRC  += $(CURDIR)/media_test/audio_player/audio_player.c
	CSRCS += $(CURDIR)/media_test/audio_player/audio_focus.c
	CSRCS += $(CURDIR)/media_test/audio_player/audio_player_utils.c
	CSRCS += $(CURDIR)/media_test/audio_player/audio_list.c
	CSRCS += $(CURDIR)/media_test/audio_player/audio_manager.c
	ifeq ($(CONFIG_LIBUV_EXTENSION),y)
		CSRCS += ./media_test/audio_player/mediatest_session.c
		PROGNAME += audioserver
		MAINSRC += $(CURDIR)/media_test/audio_player/mediatest_session_suit.c
		PROGNAME += audioplayer
		MAINSRC  += $(CURDIR)/media_test/audio_player/media_player_tool.c
	endif
	PROGNAME += mediatest_tool
	MAINSRC += $(CURDIR)/media_test/audio_player/mediatest_tool.c
	CFLAGS   += ${INCDIR_PREFIX}$(TOPDIR)
	ifeq ($(CONFIG_TELEPHONY),y)
			CFLAGS += -I$(APPDIR)/frameworks/telephony
			CSRCS += $(CURDIR)/media_test/audio_player/audio_telephony.c
	endif
	CFLAGS += -Wno-unused-function
	CFLAGS += -Wno-unused-variable
endif

ifneq  ($(CONFIG_VIDEOTEST_PLAYER),)
	ifeq ($(CONFIG_LIBUV_EXTENSION),y)
		PROGNAME += videotool
		MAINSRC += $(CURDIR)/media_test/video_player/video_test_tool.c
		PROGNAME += videoserver
		MAINSRC += $(CURDIR)/media_test/video_player/video_test_manager.c
	endif
endif

ifneq ($(CONFIG_MEDIA_FATE),)
	FFMPEG_PATH = $(APPDIR)/external/ffmpeg/ffmpeg
	CFLAGS += -I$(FFMPEG_PATH)
	CFLAGS += -I$(FFMPEG_PATH)/libavformat
	CFLAGS += -I$(FFMPEG_PATH)/libavcodec
	CFLAGS += -I$(TOPDIR)/include
	CFLAGS += -Wno-error
	LDLIBS += -lavformat -lavcodec -lavutil

	PROGNAME += url
	MAINSRC += $(FFMPEG_PATH)/libavformat/tests/url.c
	PROGNAME += seek
	MAINSRC += $(FFMPEG_PATH)/libavformat/tests/seek.c
	PROGNAME += seek_utils
	MAINSRC += $(FFMPEG_PATH)/libavformat/tests/seek_utils.c
	PROGNAME += rtmpdh
	MAINSRC += $(FFMPEG_PATH)/libavformat/tests/rtmpdh.c
	PROGNAME += noproxy
	MAINSRC += $(FFMPEG_PATH)/libavformat/tests/noproxy.c
	PROGNAME += avpacket
	MAINSRC += $(FFMPEG_PATH)/libavcodec/tests/avpacket.c
	PROGNAME += celp_math
	MAINSRC += $(FFMPEG_PATH)/libavcodec/tests/celp_math.c
	PROGNAME += codec_desc
	MAINSRC += $(FFMPEG_PATH)/libavcodec/tests/codec_desc.c
	PROGNAME += golomb
	MAINSRC += $(FFMPEG_PATH)/libavcodec/tests/golomb.c
	PROGNAME += mpeg12framerate
	MAINSRC += $(FFMPEG_PATH)/libavcodec/tests/mpeg12framerate.c
	PROGNAME += mathops
	MAINSRC += $(FFMPEG_PATH)/libavcodec/tests/mathops.c
	PROGNAME += avcodec
	MAINSRC += $(FFMPEG_PATH)/libavcodec/tests/avcodec.c
	PROGNAME += htmlsubtitles
	MAINSRC += $(FFMPEG_PATH)/libavcodec/tests/htmlsubtitles.c
	PROGNAME += mjpegenc_huffman
	MAINSRC += $(FFMPEG_PATH)/libavcodec/tests/mjpegenc_huffman.c
	PROGNAME += jpeg2000dwt
	MAINSRC += $(FFMPEG_PATH)/libavcodec/tests/jpeg2000dwt.c
	PROGNAME += pixdesc_query
	MAINSRC += $(FFMPEG_PATH)/libswscale/tests/pixdesc_query.c
	PROGNAME += floatimg_cmp
	MAINSRC += $(FFMPEG_PATH)/libswscale/tests/floatimg_cmp.c
	PROGNAME += adler32
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/adler32.c
	PROGNAME += aes
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/aes.c
	PROGNAME += aes_ctr
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/aes_ctr.c
	PROGNAME += camellia
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/camellia.c
	PROGNAME += cast5
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/cast5.c
	PROGNAME += base64
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/base64.c
	PROGNAME += blowfish
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/blowfish.c
	PROGNAME += crc
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/crc.c
	PROGNAME += fifo
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/fifo.c
	PROGNAME += hash
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/hash.c
	PROGNAME += hwdevice
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/hwdevice.c
	PROGNAME += integer
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/integer.c
	PROGNAME += lfg
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/lfg.c
	PROGNAME += uuid
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/uuid.c
	PROGNAME += twofish
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/twofish.c
	PROGNAME += eval
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/eval.c
	PROGNAME += md5
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/md5.c
	PROGNAME += opt
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/opt.c
	PROGNAME += xtea
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/xtea.c
	PROGNAME += ripemd
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/ripemd.c
	PROGNAME += sha512
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/sha512.c
	PROGNAME += sha
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/sha.c
	PROGNAME += tea
	MAINSRC += $(FFMPEG_PATH)/libavutil/tests/tea.c
endif


ifneq ($(CONFIG_CM_MEDIA_FOCUS2_TEST),)
	CFLAGS += -I$(CURDIR)/media_test/media_focus2/include
	CSRCS += $(wildcard $(CURDIR)/media_test/media_focus2/src/*.c)
	CFLAGS += -I$(CURDIR)/media_test/media_focus2/utils
	CSRCS += $(wildcard $(CURDIR)/media_test/media_focus2/utils/*.c)
	PROGNAME += cmocka_media_focus2_test
	MAINSRC += $(CURDIR)/media_test/media_focus2/media_focus2_test_main.c
endif

ifneq ($(CONFIG_CM_MEDIA_SCENARIO_TEST),)
	CFLAGS += -I$(CURDIR)/media_test/media_scenario/utils
	CSRCS += $(wildcard $(CURDIR)/media_test/media_scenario/utils/*.c)
	PROGNAME += cmocka_media_test_scenario
	MAINSRC += $(CURDIR)/media_test/media_scenario/media_scenario_test.c
	ifeq ($(CONFIG_CM_SCENARIO_PLAYBACK_TEST),y)
		CFLAGS += -I$(CURDIR)/media_test/media_scenario/playback/include
		CSRCS += $(wildcard $(CURDIR)/media_test/media_scenario/playback/src/*.c)
	endif
	ifeq ($(CONFIG_CM_SCENARIO_UV_INTERACT_TEST),y)
		CFLAGS += -I$(CURDIR)/media_test/media_scenario/interact/include
		CSRCS += $(wildcard $(CURDIR)/media_test/media_scenario/interact/src/*.c)
	endif
endif
