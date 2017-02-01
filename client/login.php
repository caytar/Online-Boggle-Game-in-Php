<?php

require ('./lib/xajax.inc.php');
session_start();


function loggedUser($ind,$formVars) {
	include 'conf.php';
	if ($ind==3) { // Log out
		$id=$_SESSION['permission'];

		unset($_SESSION['permission']);
		$text = "<iframe FRAMEBORDER=0 src =\"./login.php\" width=\"90%\" marginheight=0 height=500></iframe>";	

	}

	$objResponse = new xajaxResponse();
	$objResponse->addAssign("signup","innerHTML",$text);
	return $objResponse;

}





function loginsignup($ind,$formVars) {
	include 'conf.php';	
	$uname=trim($formVars['username']);
	$pass=trim($formVars['password']);

	if ($ind==0) { // Signup Form show
		$text="<iframe FRAMEBORDER=0 src =\"./signup.php\" width=\"90%\" marginheight=0 height=500></iframe>";
	}

	if ($ind==1) { // Login


		$uname_len=strlen($uname)-1;
		$pass_len=strlen($pass)-1;

		$gonderilecek="I000".$uname_len.$uname.$pass_len.$pass;
		
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

		if ($gelen{4}==0) {
			$text="!!!!! There is no such entry in our database with given parameters !!!!";
		}
		else	{
			$id=str2int($gelen{1})*100+str2int($gelen{2})*10+str2int($gelen{3});
// 			session_start();
			$_SESSION['permission']=$id;

			$handle3 = fopen("files/".$id.".txt", "w");
			fwrite($handle3," ");
			fclose($handle3);

			$handle2 = fopen("chats/".$id.".txt", "w");
			fwrite($handle2," ");
			fclose($handle2);
			
			$text="You have successfully loged in. You can click PLAY link on the left menu.";

		}


	}

	$objResponse = new xajaxResponse();
	$objResponse->addAssign("login","innerHTML",$text);
	return $objResponse;
}

$xajax = new xajax("login.php"); 
$xajax->registerFunction("loginsignup");
$xajax->registerFunction("loggedUser");
$xajax->processRequests();
?>


<html>
<head>
<?php $xajax->printJavascript('./'); ?>
	<link rel="stylesheet" href="css/main.css" type="text/css" media="screen" />
	<script>
		function sf(){document.search.word.focus();}
		function submit2()
		{
			xajax_loginsignup(1,xajax.getFormValues("login"));
			return false;
		}
		</script>
	
</head>
<body>

<?php 
// session_start();
if (isset($_SESSION['permission'])) 
	echo "You are logged in.";
else echo "<br><form name=\"login\" action=\"javascript:void(null)\" id=\"login\" onsubmit=\"submit2();\"><br>
		<p class=\"intro\">Login To System:</p><br>
<center><table><tr><td>
		<LABEL for=\"firstname\">User name: </LABEL></td><td>
		<input type=\"text\" class=\"txt\" name=\"username\" maxlength=10 /> </td> </tr><tr><td><LABEL for=\"firstname\">Password: </LABEL></td><td>
		<input type=\"password\" class=\"txt\" name=\"password\" maxlength=10 /></td></tr>
		<tr><td></td><td><input id=\"loginsubmit\" type=\"submit\" value=\"Login\" name=\"loginsubmit\" /><td></tr>
		</form>
		<tr></tr><tr></tr><td>or</td></tr><tr><td><a href=\"javascript:void(null)\" onclick=\"xajax_loginsignup(0,0); \">Sign-Up</a></b></td></tr></table></center>";
 ?>	
</body>
</html>