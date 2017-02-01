<?php
require ('./lib/xajax.inc.php');
session_start();

function sup($formVars) { // Signup process 	
	include 'conf.php';
	$uname=trim($formVars['uname']);
	$pass=trim($formVars['pass']);


	$uname_len=strlen($uname)-1;
	$pass_len=strlen($pass)-1;
	$gonderilecek="H000".$uname_len.$uname.$pass_len.$pass;
	
	$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	if ($socket === false)
	echo "socket_create() failed. ";

	$result = socket_connect($socket, $address, $service_port);
	if ($result === false)
	echo "socket_connect() failed.";

	$finish_bilgisi = '';

	socket_write($socket, $gonderilecek, strlen($gonderilecek));	

	while ($finish_bilgisi = socket_read($socket, $packet_size)) {
		break;
		}

	socket_close($socket);

	if ($finish_bilgisi{1}==0) 
		$text="This username exist in our database. Please change a different username";
	else
		$text="Such a user is added to our database";


	$objResponse = new xajaxResponse();
	$objResponse->addAssign("signup","innerHTML",$text);
	return $objResponse;
}

$xajax = new xajax("signup.php");
$xajax->registerFunction("sup");
$xajax->processRequests();

?>


<html>
<head>
<?php $xajax->printJavascript('./'); ?>
	<link rel="stylesheet" href="css/main.css" type="text/css" media="screen" />
	<script>
	function sf(){document.search.word.focus();}
	function submit4()
		{
			xajax_sup(xajax.getFormValues("signup"));
			return false;
		}
	</script>
	
</head>
<body onload="sf()">

<br><br>
<center>
<form name="signup" action="javascript:void(null)" id="signup" onsubmit="submit4();"><br>
		<p class="intro">Login To System:</p><br>
		
<table><tr><td><LABEL for="firstname">User name: </LABEL></td><td>
		<input type="text" class="txt" name="uname" maxlength=10 /></td></tr>
		<tr><td><LABEL for="password">Password: </LABEL></td>
		<td><input type="password" class="txt" name="pass" maxlength=10 /></td></tr>
		<tr><td></td><td><input id="signupsubmit" type="submit" value="Signup" name="signupsubmit" /></td></tr>
		</form>
</table></center>
		
</body>
</html>