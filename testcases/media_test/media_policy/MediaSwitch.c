#include <media_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

static void show_usage(void)
{
  syslog(LOG_WARNING,
         "Usage: CMD [-m] [-n>]\n"
         "\t\t-m: set policy to modem\n"
         "\t\t-n: set policy to normal\n");

  exit(0);
}

static void policy_switch_to_modem(void)
{
  media_policy_set_devices_use(MEDIA_DEVICE_MODEM);
  media_policy_set_devices_use(MEDIA_DEVICE_MIC);
  media_policy_set_devices_unuse(MEDIA_DEVICE_SCO);
  media_policy_set_devices_unavailable(MEDIA_DEVICE_SCO);
  media_policy_set_devices_unavailable(MEDIA_DEVICE_A2DP);
  media_policy_set_audio_mode(MEDIA_AUDIO_MODE_PHONE);
}

static void policy_switch_to_normal(void)
{
  media_policy_set_devices_use(MEDIA_DEVICE_MIC);
  media_policy_set_devices_unuse(MEDIA_DEVICE_MODEM);
  media_policy_set_devices_unuse(MEDIA_DEVICE_SCO);
  media_policy_set_devices_unavailable(MEDIA_DEVICE_SCO);
  media_policy_set_devices_unavailable(MEDIA_DEVICE_A2DP);
  media_policy_set_audio_mode(MEDIA_AUDIO_MODE_NORMAL);
}

int main(int argc, char *argv[])
{
  if (argc == 1)
    {
      show_usage();
    }
  int ch = 0;
  int state = 0;

  while ((ch = getopt(argc, argv, "mn")) != EOF)
    {
      switch (ch)
        {
        case 'm':
          state = 0;
          break;
        case 'n':
          state = 1;
          break;
        default:
          show_usage();
          break;
        }
    }
  if (state == 0)
    {
      policy_switch_to_modem();
    }
  else if (state == 1)
    {
      policy_switch_to_normal();
    }
  return 0;
}