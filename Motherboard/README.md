Design and manufacturing files for Talos ES™'s motherboard, which doubles as backplane board.
This is a four-layer board.<br>
<p>
<b>LAYER ORDER, from top to bottom, or first to last</b>:<br>
  <p>
<i>
TopLayer<br>
Inner1<br>
Inner2<br>
BottomLayer
</i>
<p><br>
<b>UPDATE NOTICE, 21 February 2024:</b> Two of the TVS diodes originaly used on this board (D4 and D5, used to protect the RS232 Tx and Rx lines) were not properly specified. The originals will work if you are using a modern PC as terminal for Talos ES™, but their operating voltage (5V) should have been 15V to cover all possible RS232 terminals (RS232 operates between -15 and +15V). I've now replaced these diodes with the correct ones (SA15CA-E3/54, instead of the original SA5.0CA-E3/54) in the files above. This change does not affect the PCB itself, except for the silkscreen markings. If you already had this PCB made, <i>you do NOT need to have it made again.</i> I just recommend that you then replace diodes D4 and D5 with SA15CA-E3/54 parts, instead of the original SA5.0CA-E3/54 parts, particularly if you plan to use Talos ES™ with vintage terminals.
</p>
<p><br>
<b>DISCLAIMER:</b> As per the license of this repository, these files are provided as-is. To the best of my knowledge, the board works. However, the version I actually had made and tested was slightly different from the updated version in this repository now. The changes were minimal and I have no reason to believe that this updated version doesn't work; much to the contrary. Having said that, I have not had this updated version manufactured and tested, so I cannot offer any guarantees. Just as everything in all of my Github repositories, you use these files at your own risk.

