# ##############################################################################
# apps/tests/testcases/media_test/CMakeLists.txt
#
# Licensed to the Apache Software Foundation (ASF) under one or more contributor
# license agreements.  See the NOTICE file distributed with this work for
# additional information regarding copyright ownership.  The ASF licenses this
# file to you under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#
# ##############################################################################

if(CONFIG_MEDIAS_TEST)

  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include ${NUTTX_DIR}/../apps/frameworks/media
    ${NUTTX_DIR}/../apps/frameworks/media/utils
    ${NUTTX_DIR}/../apps/external/ffmpeg/ffmpeg)

  nuttx_add_library(inner_media_test)

  set(MAINSRC "")
  set(CSRCS "")
  set(PROGNAME "")

  if(CONFIG_MEDIA_GRAPH_TEST OR CONFIG_MEDIA_STABILITY_TEST)
    list(APPEND CSRCS
         "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_test.c")
  endif()

  if(CONFIG_MEDIA_CLIENT_TEST OR CONFIG_MEDIA_SERVER_TEST)
    list(APPEND CSRCS "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/media_rpc.c")
  endif()

  if(CONFIG_MEDIA_GRAPH_TEST)
    list(APPEND CSRCS
         "${CMAKE_CURRENT_SOURCE_DIR}/media_policy/MediaPolicyTest.c")
    list(
      APPEND
      PROGNAME
      "media_graph_open"
      "media_graph_stop"
      "media_graph_pause"
      "media_stream_switch"
      "media_policy_setget"
      "media_policy_indecrease"
      "media_policy_modeChange"
      "media_policy_switch"
      "media_policy_set_volume"
      "media_loop_open"
      "media_graph_start"
      "media_graph_single_volume"
      "media_graph_mul_volume"
      "media_policy_mul_volume"
      "media_policy_single_volume"
      "media_graph_isplay_test"
      "media_graph_duration_test"
      "media_graph_seek"
      "media_graph_loop_test"
      "media_graph_record_time"
      "media_graph_reset"
      "media_loop_play"
      "media_loop_take_picture")
    list(
      APPEND
      MAINSRC
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_open.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_stop.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_pause.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_policy/MediaStreamSwitch.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_policy/MediaSetGet.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_policy/MediaInDecrease.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_policy/MediaModeChange.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_policy/MediaSwitch.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_policy/MediaVolumeAdjust.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_loop_open.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_start.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_set_volume_test.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_mul_set_volume_test.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_mul_set_stream_volume.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_set_stream_volume.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_isplay_test.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_duration_test.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_seek_and_position.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_loop_test.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_record_time.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_reset.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_loop_play.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_graph/media_graph_loop_take_picture.c")

    if(CONFIG_LIBUV_EXTENSION)
      list(
        APPEND
        PROGNAME
        "media_uv_start"
        "media_uv_stop"
        "media_uv_reset"
        "media_uv_position"
        "media_uv_pause"
        "media_uv_duration"
        "media_uv_isplaying"
        "media_uv_policy_volume"
        "media_uv_policy_dein_volume"
        "media_uv_record_time"
        "media_uv_stab_play"
        "media_uv_stab_volume"
        "media_uv_stab_pause"
        "media_uv_loop_open"
        "media_uv_stab_conflict"
        "media_uv_stab_switch"
        "media_uv_stab_seek"
        "media_uv_stab_multi_seek"
        "media_uv_stab_multi_play"
        "media_uv_set_loop"
        "media_uv_graph_volume"
        "media_uv_seek_position"
        "media_uv_loop_play"
        "media_uv_timestamp")

      list(
        APPEND
        MAINSRC
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_start.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_stop.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_reset.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_position.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_pause.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_duration.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_isplaying.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_policy_volume.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_policy_dein_volume.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_record_time.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_stab_play.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_stab_volume.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_stab_pause.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_loop_open.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_stab_conflict.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_stab_switch.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_stab_seek.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_stab_multi_seek.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_stab_multi_play.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_set_loop.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_graph_volume.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_seek_position.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_loop_play.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/media_uv/media_uv_timestamp.c")
    endif()
  endif()

  if(CONFIG_MEDIA_CLIENT_TEST)
    list(
      APPEND
      PROGNAME
      "media_rpc_client_test01"
      "media_rpc_client_test02"
      "media_rpc_client_test03"
      "media_rpc_client_test04"
      "media_rpc_client_test05"
      "media_rpc_client_test06"
      "media_rpc_client_test07"
      "media_rpc_parcel_test01"
      "media_rpc_parcel_test02")
    list(
      APPEND
      MAINSRC
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_client/media_client01.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_client/media_client02.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_client/media_client03.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_client/media_client04.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_client/media_client05.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_client/media_client06.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_client/media_client07.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_parcel/media_parcel01.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_parcel/media_parcel02.c")
  endif()

  if(CONFIG_MEDIA_SERVER_TEST)
    list(
      APPEND
      PROGNAME
      "media_rpc_server_t"
      "media_rpc_server_test01"
      "media_rpc_server_test02"
      "media_rpc_server_test03"
      "media_rpc_server_test04"
      "media_rpc_server_test05"
      "media_rpc_server_test06")
    list(
      APPEND
      MAINSRC
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_server/media_server.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_server/media_server01.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_server/media_server02.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_server/media_server03.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_server/media_server04.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_server/media_server05.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_rpc/rpc_server/media_server06.c")
  endif()

  if(CONFIG_MEDIA_FOCUS_TEST)
    list(APPEND CSRCS
         "${CMAKE_CURRENT_SOURCE_DIR}/media_focus/media_focus_test.c")
    list(
      APPEND
      PROGNAME
      "media_focus_request_api_test01"
      "media_focus_request_api_test02"
      "media_focus_request_api_test03"
      "media_focus_request_api_test04"
      "media_focus_stack_test01"
      "media_focus_stack_test02"
      "media_focus_stack_test03"
      "media_focus_stack_test04"
      "media_focus_event_test01"
      "media_focus_app_test01"
      "media_focus_app_test02"
      "media_focus_app_test03"
      "media_focus_nomemory_test01"
      "media_focus_read_matrix_test01")
    list(
      APPEND
      MAINSRC
      "${CMAKE_CURRENT_SOURCE_DIR}/media_focus/media_focus_request_api_test01.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_focus/media_focus_request_api_test02.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_focus/media_focus_request_api_test03.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_focus/media_focus_request_api_test04.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_focus/media_focus_stack_test01.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_focus/media_focus_stack_test02.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_focus/media_focus_stack_test03.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_focus/media_focus_stack_test04.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_focus/media_focus_event_test01.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_focus/media_focus_app_test01.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_focus/media_focus_app_test02.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_focus/media_focus_app_test03.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_focus/media_focus_nomemory_test01.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_focus/media_focus_read_matrix_test01.c"
    )
  endif()

  if(CONFIG_MEDIA_STABILITY_TEST)
    list(
      APPEND
      PROGNAME
      "media_stab_trans_phone"
      "media_stab_trans_blue"
      "media_stab_play_list"
      "media_stab_play_pause"
      "mediatest_stab_switch"
      "mediatest_stab_play_multi"
      "mediatest_stab_play_next"
      "media_stab_set_volume"
      "media_stab_play_seek"
      "media_play_multi_seek")
    list(
      APPEND
      MAINSRC
      "${CMAKE_CURRENT_SOURCE_DIR}/media_stability/policy_trans_phone.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_stability/policy_trans_blue.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_stability/media_play_list.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_stability/media_play_pause.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_stability/mediatest_switch.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_stability/mediatest_play_multi_pause.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_stability/media_play_next.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_stability/media_volume.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_stability/media_play_seek.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/media_stability/media_play_multi_seek.c")

    if(CONFIG_AUDIOUTILS_ALSA_LIB)
      list(APPEND PROGNAME "alsa_stab_play_list")
      list(APPEND MAINSRC
           "${CMAKE_CURRENT_SOURCE_DIR}/media_stability/media_alsa_play_list.c")
    endif()
  endif()

  if(CONFIG_MEDIATEST_TOOL)
    list(APPEND PROGNAME "mediatest")
    list(APPEND MAINSRC "${CMAKE_CURRENT_SOURCE_DIR}/media_comp/media_player.c")
  endif()

  if(CONFIG_MEDIATEST_PLAYER)
    list(APPEND PROGNAME "mediatest_player")
    list(APPEND MAINSRC
         "${CMAKE_CURRENT_SOURCE_DIR}/audio_player/audio_player.c")
    list(APPEND CSRCS "${CMAKE_CURRENT_SOURCE_DIR}/audio_player/audio_focus.c"
         "${CMAKE_CURRENT_SOURCE_DIR}/audio_player/audio_list.c"
         "${CMAKE_CURRENT_SOURCE_DIR}/audio_player/audio_player_utils.c"
         "${CMAKE_CURRENT_SOURCE_DIR}/audio_player/audio_manager.c")
    if(CONFIG_LIBUV_EXTENSION)
      list(APPEND CSRCS
           "${CMAKE_CURRENT_SOURCE_DIR}/audio_player/mediatest_session.c")
      list(APPEND PROGNAME "audioserver" "audioplayer")
      list(APPEND MAINSRC
           "${CMAKE_CURRENT_SOURCE_DIR}/audio_player/mediatest_session_suit.c"
           "${CMAKE_CURRENT_SOURCE_DIR}/audio_player/media_player_tool.c")
    endif()
    list(APPEND PROGNAME "mediatest_tool")
    list(APPEND MAINSRC
         "${CMAKE_CURRENT_SOURCE_DIR}/audio_player/mediatest_tool.c")
    if(CONFIG_TELEPHONY)
      include_directories(${APPDIR}/frameworks/telephony)
      list(APPEND CSRCS
           "${CMAKE_CURRENT_SOURCE_DIR}/audio_player/audio_telephony.c")
    endif()
    add_compile_options(-Wno-unused-function -Wno-unused-variable)
  endif()

  if(CONFIG_VIDEOTEST_PLAYER)
    if(CONFIG_LIBUV_EXTENSION)
      list(APPEND PROGNAME "videotool" "videoserver")
      list(APPEND MAINSRC
           "${CMAKE_CURRENT_SOURCE_DIR}/video_player/video_test_tool.c"
           "${CMAKE_CURRENT_SOURCE_DIR}/video_player/video_test_manager.c")
    endif()
  endif()

  target_sources(inner_media_test PRIVATE ${CSRCS})
  list(LENGTH MAINSRC SOURCE_FILES_COUNT)
  math(EXPR LAST_INDEX "${SOURCE_FILES_COUNT} - 1")
  foreach(INDEX RANGE 0 ${LAST_INDEX})
    list(GET MAINSRC ${INDEX} SOURCE_FILE)
    list(GET PROGNAME ${INDEX} EXECUTABLE_NAME)
    nuttx_add_application(
      NAME
      ${EXECUTABLE_NAME}
      PRIORITY
      ${CONFIG_TESTING_TESTCASES_PRIORITY}
      STACKSIZE
      ${CONFIG_TESTING_TESTCASES_STACKSIZE}
      SRCS
      ${SOURCE_FILE}
      MODULE
      ${CONFIG_TESTS_TESTCASES})
  endforeach()

  if(CONFIG_MEDIA_FATE)
    include_directories(${CMAKE_BINARY_DIR}/apps/external/ffmpeg)

    set(FFMPEG_PATH ${NUTTX_DIR}/../apps/external/ffmpeg/ffmpeg)
    set(FATEMAINSRC "")
    set(FATEPROGNAME "")

    list(
      APPEND
      FATEPROGNAME
      "url"
      "seek"
      "seek_utils"
      "rtmpdh"
      "noproxy"
      "avpacket"
      "celp_math"
      "codec_desc"
      "golomb"
      "mpeg12framerate"
      "mathops"
      "avcodec"
      "htmlsubtitles"
      "mjpegenc_huffman"
      "jpeg2000dwt"
      "pixdesc_query"
      "floatimg_cmp"
      "adler32"
      "aes"
      "aes_ctr"
      "camellia"
      "cast5"
      "base64"
      "blowfish"
      "crc"
      "fifo"
      "hash"
      "hwdevice"
      "integer"
      "lfg"
      "uuid"
      "twofish"
      "eval"
      "md5"
      "opt"
      "xtea"
      "ripemd"
      "sha512"
      "sha"
      "tea")

    list(
      APPEND
      FATEMAINSRC
      "${FFMPEG_PATH}/libavformat/tests/url.c"
      "${FFMPEG_PATH}/libavformat/tests/seek.c"
      "${FFMPEG_PATH}/libavformat/tests/seek_utils.c"
      "${FFMPEG_PATH}/libavformat/tests/rtmpdh.c"
      "${FFMPEG_PATH}/libavformat/tests/noproxy.c"
      "${FFMPEG_PATH}/libavcodec/tests/avpacket.c"
      "${FFMPEG_PATH}/libavcodec/tests/celp_math.c"
      "${FFMPEG_PATH}/libavcodec/tests/codec_desc.c"
      "${FFMPEG_PATH}/libavcodec/tests/golomb.c"
      "${FFMPEG_PATH}/libavcodec/tests/mpeg12framerate.c"
      "${FFMPEG_PATH}/libavcodec/tests/mathops.c"
      "${FFMPEG_PATH}/libavcodec/tests/avcodec.c"
      "${FFMPEG_PATH}/libavcodec/tests/htmlsubtitles.c"
      "${FFMPEG_PATH}/libavcodec/tests/mjpegenc_huffman.c"
      "${FFMPEG_PATH}/libavcodec/tests/jpeg2000dwt.c"
      "${FFMPEG_PATH}/libswscale/tests/pixdesc_query.c"
      "${FFMPEG_PATH}/libswscale/tests/floatimg_cmp.c"
      "${FFMPEG_PATH}/libavutil/tests/adler32.c"
      "${FFMPEG_PATH}/libavutil/tests/aes.c"
      "${FFMPEG_PATH}/libavutil/tests/aes_ctr.c"
      "${FFMPEG_PATH}/libavutil/tests/camellia.c"
      "${FFMPEG_PATH}/libavutil/tests/cast5.c"
      "${FFMPEG_PATH}/libavutil/tests/base64.c"
      "${FFMPEG_PATH}/libavutil/tests/blowfish.c"
      "${FFMPEG_PATH}/libavutil/tests/crc.c"
      "${FFMPEG_PATH}/libavutil/tests/fifo.c"
      "${FFMPEG_PATH}/libavutil/tests/hash.c"
      "${FFMPEG_PATH}/libavutil/tests/hwdevice.c"
      "${FFMPEG_PATH}/libavutil/tests/integer.c"
      "${FFMPEG_PATH}/libavutil/tests/lfg.c"
      "${FFMPEG_PATH}/libavutil/tests/uuid.c"
      "${FFMPEG_PATH}/libavutil/tests/twofish.c"
      "${FFMPEG_PATH}/libavutil/tests/eval.c"
      "${FFMPEG_PATH}/libavutil/tests/md5.c"
      "${FFMPEG_PATH}/libavutil/tests/opt.c"
      "${FFMPEG_PATH}/libavutil/tests/xtea.c"
      "${FFMPEG_PATH}/libavutil/tests/ripemd.c"
      "${FFMPEG_PATH}/libavutil/tests/sha512.c"
      "${FFMPEG_PATH}/libavutil/tests/sha.c"
      "${FFMPEG_PATH}/libavutil/tests/tea.c")

    add_compile_options(-Wno-error -Wno-shadow -Wno-parentheses -Wno-pointer-sign)
    list(LENGTH FATEMAINSRC FATE_SOURCE_FILES_COUNT)
    math(EXPR FATE_LAST_INDEX "${FATE_SOURCE_FILES_COUNT} - 1")
    foreach(INDEX RANGE 0 ${FATE_LAST_INDEX})
      list(GET FATEMAINSRC ${INDEX} FATE_SOURCE_FILE)
      list(GET FATEPROGNAME ${INDEX} FATE_EXECUTABLE_NAME)
      nuttx_add_application(
        NAME
        ${FATE_EXECUTABLE_NAME}
        PRIORITY
        ${CONFIG_TESTING_TESTCASES_PRIORITY}
        STACKSIZE
        ${CONFIG_TESTING_TESTCASES_STACKSIZE}
        SRCS
        ${FATE_SOURCE_FILE}
        DEPENDS
        ffmpeg
        MODULE
        ${CONFIG_TESTS_TESTCASES})
    endforeach()
  endif()

endif()
