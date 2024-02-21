# Talos ES™
![IMG_6748](https://user-images.githubusercontent.com/69539226/226492141-28d48328-b168-4ef7-87dc-6f37a28292a1.jpeg)

<p>
Talos ES™ is a mini-computer built with standard, discrete logic (74-series chips) and featuring a custom RISC CPU. It's meant for education and entertainment. The project's official website can be found <a href="https://www.thebyteattic.com/p/talos-es.html">here</a>.<br><br>
This repository accompanies the technical material discussed in the episodes of a <a href="https://www.youtube.com/watch?v=509XYuB6xsw&list=PLDf2uklC__d0CCgEDWJ5CoJgBmkGZ0vGv&ab_channel=TheByteAttic">video series chronicling the design</a>.<br><br>
For an architecture overview of the Talos™ CPU, check out the <a href="https://github.com/TheByteAttic/TalosES/blob/main/TalosES%20Programmers%20Guide.pdf">Programmers Guide</a>. For a complete specification of its Instruction Set, check the directory <a href="https://github.com/TheByteAttic/TalosES/tree/main/ISA">/ISA</a>.<br><br>
The Talos ES™ project is now complete and all the relevant material is uploaded here. However, the documentation is very limited and sometimes non-existent. The reason is that I regard Talos ES™ mostly as a prototype for the upcoming, single-board, SMD-based, Cost-Reduced, Talos CR™ version. I expect only very few people to attempt to build this prototype, due to its high cost and high complexity. Indeed, building a Talos ES™ should be attempted only by hobbyists and students with plenty of experience not only with digital electronics in general, but ATmega microcontrollers and the Arduino IDE in particular. These are people who, having watched the video series, will hardly need any documentation anyway. If you are a beginner in electronics, please save yourself a lot of trouble and start with some other, simpler project.<br><br>
<b>CHANGE HISTORY:</b><br>
  <UL>
    <LI><b>21 February 2024</b>: Two of the TVS diodes originaly used on this board (to protect the RS232 Tx and Rx lines) were not properly specified. The originals will work if you are using a modern PC as terminal for Talos ES™, but their operating voltage (5V) should have been 15V to cover all possible RS232 terminals (RS232 operates between -15 and +15V). I've now replaced these diodes with the correct ones (SA15CA-E3/54, instead of the original SA5.0CA-E3/54) in the files above. This change does not affect the PCB itself, except for the silkscreen markings. If you already had this PCB made, <i>you do NOT need to have it made again.</i> I just recommend that you then replace diodes D4 and D5 with SA15CA-E3/54 parts, instead of the original SA5.0CA-E3/54 parts, particularly if you plan to use Talos ES™ with vintage terminals.</LI>
    <LI><b>20 November 2023</b>: Correction to page 7 of the Programmers Guide.</LI>
    <LI><b>19 November 2023</b>: First complete release.</LI>
  </UL>
<hr>
Copyright © 2022-2024 by Bernardo Kastrup. All rights reserved.
