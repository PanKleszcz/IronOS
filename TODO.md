# TODO — HS-02A IronOS follow-up work

> Status (2026-07-23): branch `fix/hs02-pid-tuning`. PID is tuned (holds set point within
> 1°C under load). Temperature model uses a 26µV/°C constant calibrated from solder melting
> points (absolute error ~+17°C, vs +47°C on stock firmware). 430°C now solders XT60
> connectors fine. Further accuracy gains need real measurement hardware.

## 1. Buy/build a tip thermometer (prerequisite)

Two options:
- **Off the shelf**: an FG-100-style tip thermometer (clone, ~$5-12, K-type
  thermocouple + sensing strip consumable).
- **DIY**: MAX31855 module (built-in cold-junction compensation, ±2°C) + a
  0.1-0.2mm fine-wire K-type thermocouple + ESP32/Pico, total ~$3-5. Fine wire
  matters: small thermal mass means fast response and it won't sink heat away
  from the tip. Touch the tip to the bead and add a dab of solder as a thermal
  bridge.

## 2. Once you have a thermometer: real three-point calibration

Goal: bring the ~+17°C residual down to within ±3°C. Either path works:

- **Path A (no code changes)**: reflash stock firmware, use its 150/250/350°C
  offset calibration screen to calibrate against the thermometer. The
  calibration values get written to flash at `0x0801F800`; reflashing IronOS
  afterward auto-detects and adopts them (`ThermoModel.cpp`) — the debug menu's
  "Tip Cal" page flips from "Unused 26uV/C" to "In Use" once it takes effect.
- **Path B (better UX)**: implement our own three-point calibration menu in
  IronOS that writes the same stock page format (word = count + bias, see
  CLAUDE.md for the bias constants) — compatible with stock firmware either way.

Verify by redoing the solder-melting-point test: after calibration, Sn45/Pb55
should cut around a displayed ~183°C and fully melt around ~227°C.

## 3. Hardware mystery: T_SENSE amplifier gain

Measure the feedback resistor on the AD8552 temperature channel with a
multimeter: 48K (gain 49) or 56K (gain 57)? The KiCad-redrawn schematic shows
both values and they may have been mixed up between the two amp channels.
Update the CLAUDE.md hardware section once measured. (Doesn't affect
measurement accuracy — the gain constant cancels out in both temperature-model
code paths; this is purely archaeological.)

## 4. Upstream PR

- The `fix(pid): conditional anti-windup + configurable integral clamp` commit
  (e1f34028) is worth submitting to Ralim/IronOS on its own — every board using
  the shared PID controller (Pinecilv2, etc.) benefits from eliminating the
  slow limit-cycle around the set point after heatup/overshoot.

## 5. Misc

- [ ] Push `fix/hs02-pid-tuning` to the fork.
- [ ] (Optional) Runtime ADRC/PID switch menu — the original idea, superseded
      by just switching to PID; only worth doing for A/B comparison.
- [ ] (Optional) Once calibration accuracy improves, revisit whether KD=8000
      is still the optimal damping value.
