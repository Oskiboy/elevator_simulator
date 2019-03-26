# Elevator simulator

## Build

To build run

```bash
./build.sh
```

## Running server

To run the server just run the binary:

```bash
./build/sim_server
```

This will open a little terminal with little to no functionality, but to shut the server down, write enter 'q' or 'quit'

## Command set

```bash
python socket_server.py [id <integer>] get/set cmd <value>
#Available commands:
    BUTTON_COMMAND_1,
    BUTTON_COMMAND_2,
    BUTTON_COMMAND_3,
    BUTTON_COMMAND_4,
    BUTTON_UP_1,
    BUTTON_UP_2,
    BUTTON_UP_3,
    BUTTON_DOWN_2,
    BUTTON_DOWN_3,
    BUTTON_DOWN_4,
    STOP_BUTTON,
    OBSTRUCTION,
    FLOOR_SENSOR,
    LIGHT_STOP,
    LIGHT_COMMAND_1,
    LIGHT_COMMAND_2,
    LIGHT_COMMAND_3,
    LIGHT_COMMAND_4,
    LIGHT_UP_1,
    LIGHT_UP_2,
    LIGHT_UP_3,
    LIGHT_DOWN_2,
    LIGHT_DOWN_3,
    LIGHT_DOWN_4,
    LIGHT_FLOOR_1,
    LIGHT_FLOOR_2,
    LIGHT_FLOOR_3,
    LIGHT_FLOOR_4,
    MOTOR_DIR,
    MOTOR_SPEED
```

Example:

```bash
python socket_server.py get STOP_BUTTON
```

For use in code send TCP socket messages as: "get STOP_BUTTON" or "set LIGHT_STOP 1"

## New commands set

To make this command set correspond with a different simulator I am going to adopt this command set:

<table>
    <tbody>
        <tr>
            <td><strong>Writing</strong></td>
            <td align="center" colspan="4">Instruction</td>
            <td align="center" colspan="0" rowspan="7"></td>
        </tr>
        <tr>
            <td><em>Reload config (file and args)</em></td>
            <td>&nbsp;&nbsp;0&nbsp;&nbsp;</td>
            <td>X</td>
            <td>X</td>
            <td>X</td>
        </tr>
        <tr>
            <td><em>Motor direction</em></td>
            <td>&nbsp;&nbsp;1&nbsp;&nbsp;</td>
            <td>direction<br>[-1 (<em>255</em>),0,1]</td>
            <td>X</td>
            <td>X</td>
        </tr>
        <tr>
            <td><em>Order button light</em></td>
            <td>&nbsp;&nbsp;2&nbsp;&nbsp;</td>
            <td>button<br>[0,1,2]</td>
            <td>floor<br>[0..NF]</td>
            <td>value<br>[0,1]</td>
        </tr>
        <tr>
            <td><em>Floor indicator</em></td>
            <td>&nbsp;&nbsp;3&nbsp;&nbsp;</td>
            <td>floor<br>[0..NF]</td>
            <td>X</td>
            <td>X</td>
        </tr>
        <tr>
            <td><em>Door open light</em></td>
            <td>&nbsp;&nbsp;4&nbsp;&nbsp;</td>
            <td>value<br>[0,1]</td>
            <td>X</td>
            <td>X</td>
        </tr>
        <tr>
            <td><em>Stop button light</em></td>
            <td>&nbsp;&nbsp;5&nbsp;&nbsp;</td>
            <td>value<br>[0,1]</td>
            <td>X</td>
            <td>X</td>
        </tr>
        <tr>
            <td><strong>Reading</strong></td>
            <td align="center" colspan="4">Instruction</td>
            <td></td>
            <td align="center" colspan="4">Output</td>
        </tr>
        <tr>
            <td><em>Order button</em></td>
            <td>&nbsp;&nbsp;6&nbsp;&nbsp;</td>
            <td>button<br>[0,1,2]</td>
            <td>floor<br>[0..NF]</td>
            <td>X</td>
            <td align="right"><em>Returns:</em></td>
            <td>6</td>
            <td>pressed<br>[0,1]</td>
            <td>0</td>
            <td>0</td>
        </tr>
        <tr>
            <td><em>Floor sensor</em></td>
            <td>&nbsp;&nbsp;7&nbsp;&nbsp;</td>
            <td>X</td>
            <td>X</td>
            <td>X</td>
            <td align="right"><em>Returns:</em></td>
            <td>7</td>
            <td>at floor<br>[0,1]</td>
            <td>floor<br>[0..NF]</td>
            <td>0</td>
        </tr>
        <tr>
            <td><em>Stop button</em></td>
            <td>&nbsp;&nbsp;8&nbsp;&nbsp;</td>
            <td>X</td>
            <td>X</td>
            <td>X</td>
            <td align="right"><em>Returns:</em></td>
            <td>8</td>
            <td>pressed<br>[0,1]</td>
            <td>0</td>
            <td>0</td>
        </tr>
        <tr>
            <td><em>Obstruction switch</em></td>
            <td>&nbsp;&nbsp;9&nbsp;&nbsp;</td>
            <td>X</td>
            <td>X</td>
            <td>X</td>
            <td align="right"><em>Returns:</em></td>
            <td>9</td>
            <td>active<br>[0,1]</td>
            <td>0</td>
            <td>0</td>
        </tr>
        <tr>
            <td colspan="0"><em>NF = Num floors. X = Don't care.</em></td>
        </tr>
    </tbody>
</table>
