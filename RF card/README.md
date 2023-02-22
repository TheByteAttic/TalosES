This directory contains the design and manufacturing files of Talos ES™'s RF card, as well as reference pictures to help you assemble your own.<br>
<p>
<b>UPDATE</b>, 2023-02-22: This PCB initially had two layers. The board illustrated in the "/photos" directory, as well as that shown in the video series, are instances of that initial two-layer version. Since then, other Talos ES™ cards had to be designed as four-layer boards (two signal layers plus two inner planes for power and ground). The difference in price for a four-layer board also turns out to be relatively insignificant in the context of a large system like Talos ES™. So I've now migrated the RF card in this directory to four layers as well. The design files in this directory are now for the updated four-layer board with the originally missing resistor bank.<br>
<p>
<b>DISCLAIMER</b>: <i>I have NOT respun this board myself (as the original works well enough), so the new version in the files above is, strictly speaking, partly untested.</i> That said, the changes are minimal and the functionality identical to the tested original. Automatic tool checks have also been performed (all nets are checked as routed, and the new version is DRC-clean). Therefore, there is no (new) reason to think that this new version won't work; if anything, it will work better, due to the better electrical characteristics of the two inner planes (for power and ground). Still, just as always, I do NOT take responsibility for any mishaps or losses you may incur if you decide to make this board, or have it made for you. If in doubt, check the more rigorous language in this repository's license file.<br>
<p>
<b>LAYER ORDER, from top to bottom, or first to last</b>:<br>
  <p>
<i>
TopLayer<br>
Inner1<br>
Inner2<br>
BottomLayer
</i>
