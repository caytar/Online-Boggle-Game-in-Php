<?php
require ('./lib/xajax.inc.php');
session_start();

function croom($formVars) { // Signup process 	
	include 'conf.php';
	$rname=trim($formVars['rname']);
	$row=trim($formVars['row']);
	$col=trim($formVars['col']);
	$rtime=trim($formVars['rtime']);
	$rw1=trim($formVars['rw1']);
	$rw2=trim($formVars['rw2']);
	$rw3=trim($formVars['rw3']);
	$rw4=trim($formVars['rw4']);
	$rw5=trim($formVars['rw5']);
	$rw6=trim($formVars['rw6']);
	$rw7=trim($formVars['rw7']);
	$rw8=trim($formVars['rw8']);
	$rw9=trim($formVars['rw9']);
	$rw10=trim($formVars['rw10']);


	$rname_len=strlen($rname)-1;
	$rtime_len=strlen($rtime);
	$rw1_len=strlen($rw1);
	$rw2_len=strlen($rw2);
	$rw3_len=strlen($rw3);
	$rw4_len=strlen($rw4);
	$rw5_len=strlen($rw5);
	$rw6_len=strlen($rw6);
	$rw7_len=strlen($rw7);
	$rw8_len=strlen($rw8);
	$rw9_len=strlen($rw9);
	$rw10_len=strlen($rw10);

	$gonderilecek="A000".$rname_len.$rname.$row.$col.$rtime_len.$rtime.$rw1_len.$rw1.$rw2_len.$rw2.$rw3_len.$rw3.$rw4_len.$rw4.$rw5_len.$rw5.$rw6_len.$rw6.$rw7_len.$rw7.$rw8_len.$rw8.$rw9_len.$rw9.$rw10_len.$rw10;
	
	$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	if ($socket === false)
	echo "socket_create() failed. ";

	$result = socket_connect($socket, $address, $service_port);
	if ($result === false)
	echo "socket_connect() failed.";

	$gelen = '';

	socket_write($socket, $gonderilecek, strlen($gonderilecek));	

	while ($gelen = socket_read($socket, $packet_size)) {
		break;
		}

	socket_close($socket);

	if ($gelen{1}==0) 
		$text="Because of an error, room couldn't be created !!!";
	else
		$text="Room has been created";


	$objResponse = new xajaxResponse();
	$objResponse->addAssign("croomdiv","innerHTML",$text);
	return $objResponse;
}

$xajax = new xajax("croom.php");
$xajax->registerFunction("croom");
$xajax->processRequests();

?>


<html>
<head>
<?php $xajax->printJavascript('./'); ?>
	<link rel="stylesheet" href="css/main.css" type="text/css" media="screen" />
	<script>
	function sf(){document.search.word.focus();}
	function submit8()
		{
			xajax_croom(xajax.getFormValues("croom"));
			return false;
		}
	</script>
	
</head>
<body onload="sf()">

<br><br>
<center>
<div id="croomdiv">
<form name="croom" action="javascript:void(null)" id="croom" onsubmit="submit8();"><br>
		<p class="intro">Create Room:</p><br>
		
<table><tr><td><LABEL for="roomname">Room name: </LABEL></td><td>
		<input type="text" class="txt" name="rname" maxlength=10 size=10 /></td></tr>
		<tr><td><LABEL for="rovxcol">RowxCol: </LABEL></td>
		<td><input type="text" class="txt" name="row" maxlength=1 size=1 />x<input type="text" class="txt" name="col" maxlength=1 size=1 /></td></tr>
<tr><td><LABEL for="rtime">Game Duration: </LABEL></td><td>
		<input type="text" class="txt" name="rtime" maxlength=3 size=3 /> seconds</td></tr>
<tr><td><LABEL for="rtime">Points according to word length: </LABEL></td><td>
<table>
<tr><td>1 length word : <input type="text" class="txt" name="rw1" maxlength=2 size=2 />points</td></tr>
<tr><td>2 length word : <input type="text" class="txt" name="rw2" maxlength=2 size=2 />points</td></tr>
<tr><td>3 length word : <input type="text" class="txt" name="rw3" maxlength=2 size=2 />points</td></tr>
<tr><td>4 length word : <input type="text" class="txt" name="rw4" maxlength=2 size=2 />points</td></tr>
<tr><td>5 length word : <input type="text" class="txt" name="rw5" maxlength=2 size=2 />points</td></tr>
<tr><td>6 length word : <input type="text" class="txt" name="rw6" maxlength=2 size=2 />points</td></tr>
<tr><td>7 length word : <input type="text" class="txt" name="rw7" maxlength=2 size=2 />points</td></tr>
<tr><td>8 length word : <input type="text" class="txt" name="rw8" maxlength=2 size=2 />points</td></tr>
<tr><td>9 length word : <input type="text" class="txt" name="rw9" maxlength=2 size=2 />points</td></tr>
<tr><td>10 length word : <input type="text" class="txt" name="rw10" maxlength=2 size=2 />points</td></tr>

</table>
</td></tr>
<tr><td></td><td><input id="csubmit" type="submit" value="Create Room" name="csubmit" /></td></tr>
</form>
</table>
</div>
</center>
		
</body>
</html>