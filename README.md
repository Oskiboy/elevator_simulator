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

or with a commandline:

```bash
./build/server_commander
```

This will open a little terminal with little to no functionality, but to shut the server down, write enter 'q' or 'quit'

### Server commander

Commands in the commandline:

```bash
>>> pos
>>> cmd X X X X
```

`pos` will print the current position.
`cmd X X X X` will send the command given to the elevator.

## Command set

```bash
python socket_server.py -p <port> [cmd]
```

Example:

```bash
python socket_server.py 1 1 0 0
```

For use in code send TCP socket messages send four bytes with the command.

## Command set:

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

## Extra commands for emulated elevator interaction

### TODO:

This should be expanded with: resetting elevator, setting position, 

<table>
    <tbody>
        <tr>
            <td><strong>System Commands</strong></td>
            <td align="center" colspan="4">Instruction</td>
            <td></td>
            <td align="center" colspan="4">Output</td>
        </tr>
        <tr>
            <td><em>Push button</em></td>
            <td>&nbsp;&nbsp;10&nbsp;&nbsp;</td>
            <td>type<br>[0..4]</td>
            <td>floor<br>[0..NF]</td>
            <td>duration(in 10 ms)<br>[0..255]</td>
        </tr>
            <td colspan="0"><em>Type: [0,1,2] are for buttons, 3 is stop and 4 is for the obstruction button.</em></td>
        <tr>
            <td><em>Get position</em></td>
            <td>&nbsp;&nbsp;255&nbsp;&nbsp;</td>
            <td>X</td>
            <td>X</td>
            <td>X</td>
            <td align="right"><em>Returns:</em></td>
            <td>255</td>
            <td>0</td>
            <td>0</td>
            <td>0</td>
        </tr>
        <tr>
            <td colspan="0"><em>After the 4 bytes, read one double as well to get position.</em></td>
        </tr>
    </tbody>
</table>
