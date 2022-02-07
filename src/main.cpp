#include <Arduino.h>
#include <psyduck-base.h>
#include <psyduck-homie.h>
#include <psyduck-role-thermostat.h>

void setup() {
  psyduck::base::setup();
}

void loop() {
  psyduck::base::Timers::tick();
}