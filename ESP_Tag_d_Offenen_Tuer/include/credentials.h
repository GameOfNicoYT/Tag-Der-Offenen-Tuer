#ifndef __CREDENTIALS_H__
#define __CREDENTIALS_H__
// WIFI configuration, supports more than one WIFI, first found first served
struct CREDENTIAL
{
  char const *ssid;
  char const *username;
  char const *password;
  char const *mqtt;
};

CREDENTIAL currentWifi; // global to store found wifi

// * = Passwort

std::vector<CREDENTIAL> const credentials = {
    {"Proyer", "", "Unterrohr264", ""},
    {"IPHONE", "", "Proyer08", ""},
    {"htlpinkafeld", "nico.proyer", "Unterrohr!264_2023", ""}};
#endif
