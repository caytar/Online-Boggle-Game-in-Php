<?php
require ('./lib/xajax.inc.php');
session_start();

function isFinished() { // Is the game finished?
	include 'conf.php';
	$user=$_SESSION['permission'];

	$yazdirilacak="";
	$tekrar="<font color=red><i><-- Click PLAY to continue</i></font>";

	$objResponse = new xajaxResponse();

	$handle2 = fopen("games/bitenoyun".$user.".txt", "r");
	$fileinfo=fscanf($handle2, "%d");
	list ($bitti_flag) = $fileinfo;	
	fclose($handle2);


	$handle2 = fopen("games/current".$_SESSION['permission'].".txt", "r");
	$fileinfo=fscanf($handle2, "%s");
	list ($room_name) = $fileinfo;	
	fclose($handle2);

	if (strlen($user)==1) {
		$user="00".$user;
		}
	else if (strlen($user)==2) {
			$user="0".$user;
		}
	$room_len=strlen($room_name)-1;

	if ($bitti_flag==0) {
		$gonderilecek="E".$user.$room_len.$room_name;
		
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

		if ($finish_bilgisi{$room_len+6}=="0") {
			$handle = fopen("games/bitenoyun".$_SESSION['permission'].".txt", "w");
			fwrite($handle, "5 ");
			fclose($handle);	
		}


	}
	else if ($bitti_flag==5) {


		$yazdirilacak="Game is Over<br>Calculating the points";

		$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		if ($socket === false)
			echo "socket_create() failed. ";

		$result = socket_connect($socket, $address, $service_port);
		if ($result === false)
			echo "socket_connect() failed.";
		
		$sonuclar_ciktimi="F".$user.$room_len.$room_name;
		$sonuc_listesi = '';

		socket_write($socket, $sonuclar_ciktimi, strlen($sonuclar_ciktimi));	

		while ($sonuc_listesi = socket_read($socket, $packet_size)) {
			break;
		}

		socket_close($socket);
		
		
		if ($sonuc_listesi{$room_len+6}=="1") {
			$yazdirilacak="<table border=1 width=100%><tr><td>Player</td><td>Points</td><td>Overall Ranking</td></tr>";
			$oyuncu_sayisi_len=str2int($sonuc_listesi{$room_len+7});
			$k=0;
			$m=0;
			$oyuncu_sayisi=0;
			if ($m<=$oyuncu_sayisi_len) {
			$oyuncu_sayisi=$oyuncu_sayisi+str2int($sonuc_listesi{$room_len+8+$m})*usal(10,$oyuncu_sayisi_len-$m);
			
			}

			$my_pointer=$room_len+9+$oyuncu_sayisi_len;

		
			while ($k<$oyuncu_sayisi) {
				
				$user2=str2int($sonuc_listesi{$my_pointer})*100+str2int($sonuc_listesi{$my_pointer+1})*10+str2int($sonuc_listesi{$my_pointer+2});
				
				if (strlen($user2)==1) {
					$user2="00".$user2;
					}
				else if (strlen($user2)==2) {
						$user2="0".$user2;
					}

				$gonderilecek="J".$user2;
				
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

				$uname_len=str2int($gelen{1});
				$uname=substr($gelen,2,$uname_len+1);

				$upuan=str2int($sonuc_listesi{$my_pointer+3})*100+str2int($sonuc_listesi{$my_pointer+4})*10+str2int($sonuc_listesi{$my_pointer+5});
				$urank=str2int($sonuc_listesi{$my_pointer+6})*100+str2int($sonuc_listesi{$my_pointer+7})*10+str2int($sonuc_listesi{$my_pointer+8});

				$yazdirilacak=$yazdirilacak."<tr><td>".$uname."</td><td>".$upuan."</td><td>".$urank."</td></tr>";
				$my_pointer=$my_pointer+9;
				$k++;


				}


			$yazdirilacak=$yazdirilacak."</table>  ";

			$handle = fopen("games/bitenoyun".$_SESSION['permission'].".txt", "w");
			fwrite($handle, "1 ".$yazdirilacak);
			fclose($handle);	
			$sonflag=777;
		}

	}
	else {
		$handle2 = fopen("games/bitenoyun".$_SESSION['permission'].".txt", "r");
		$dosyadan='';
		while (!feof($handle2))
			$dosyadan .= fread($handle2, 8192);			
		fclose($handle2);
		$yazdirilacak=substr($dosyadan,2,strlen($dosyadan)-3);
		$objResponse->addAssign("players","innerHTML",$yazdirilacak);
	}
	

	
	$objResponse->addAssign("playagain","innerHTML",$tekrar);



	return $objResponse;
}

function receiveChat()  {
	include 'conf.php';
	$user=$_SESSION['permission'];
	$filename="chats/".$user.".txt";

	$handle2 = fopen($filename,"r");
	$dosyadan=fread($handle2, filesize($filename));
	fclose($handle2);

	$objResponse = new xajaxResponse();
	$objResponse->addAssign("chat","innerHTML",$dosyadan);
	return $objResponse;
}

function guessWord2($formVars) {
	include 'conf.php';


	if (isset($_SESSION['permission'])) {
	$user=$_SESSION['permission'];

	if (strlen($user)==1) {
		$user="00".$user;
	}
	else 
		if (strlen($user)==2) {
			$user="0".$user;
		}


	$gonderilecek="J".$user;
	
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


	
	$uname_len=str2int($gelen{1})+1;
	$uname=substr($gelen,2,$uname_len);


	$chatMssg=trim($formVars['chatMsg']);
	$toWhom=trim($formVars['smenu']);

	if ($toWhom==0) {
	
		$handle2 = fopen("games/current".$_SESSION['permission'].".txt", "r");
		$fileinfo=fscanf($handle2, "%s");
		list ($room_name) = $fileinfo;	
		fclose($handle2);

		$room_name_len=strlen($room_name)-1;
		$gonderilecek="K000".$room_name_len.$room_name;
		
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

		$active_users_len=str2int($gelen{1});


		
		$active_users=0;
		for ($c=0;$c<$active_users_len;$c++) {
			$active_users=$active_users+str2int($gelen{2+$c})*usal(10,$active_users_len-$c-1);
		}

		$nerdeyim=2+$active_users_len;
		$c=0;
		while ($c<$active_users) {

		$pname_len=$gelen{$nerdeyim+3};
		$id=str2int($gelen{$nerdeyim})*100+str2int($gelen{$nerdeyim+1})*10+str2int($gelen{$nerdeyim+2});
		$filename="chats/".$id.".txt";

		$handle2 = fopen($filename,"r");
		$dosyadan=fread($handle2, filesize($filename));
		fclose($handle2);

		$dosyadan="<font color=red>".$uname." :</font> ".$chatMssg.'<br>'.$dosyadan;
		$handle = fopen($filename, "w");
		fwrite($handle, $dosyadan);
		fclose($handle);
		$nerdeyim=$nerdeyim+5+$pname_len;
		$c++;

		}


	}
	else {
		$filename="chats/".$toWhom.".txt";

		$handle2 = fopen($filename,"r");
		$dosyadan=fread($handle2, filesize($filename));
		fclose($handle2);

		$dosyadan="<font color=red>".$uname." :</font> ".$chatMssg.'<br>'.$dosyadan;
		$handle = fopen($filename, "w");
		fwrite($handle, $dosyadan);
		fclose($handle);
	}

}

	$objResponse = new xajaxResponse();
	$objResponse->addAssign("chatMsg","value","");
	return $objResponse;

}


$xajax = new xajax("chat.php"); 
$xajax->registerFunction("guessWord2");
$xajax->registerFunction("receiveChat");
$xajax->registerFunction("isFinished");
$xajax->processRequests();
?>



<html>
<head>
<?php $xajax->printJavascript('./'); ?>
	<link rel="stylesheet" href="css/main.css" type="text/css" media="screen" />
		<script type="text/javascript">
		var current="Sorry! No Time"
		var thetimeout
		var gondereyimmi=1
		
		function submitChat()
		{
			xajax_guessWord2(xajax.getFormValues("chatbuton"));
			return false;
		}
		function oyunbittimi()
		{
			xajax_isFinished()
			return false;
		}
		
		function countdown(min,sec){
			thesec=sec;
			themin=min;

			if(thesec==0 && themin==0){
				document.forms.count.count2.value=current
				if (gondereyimmi==1)
					{
					oyunbittimi()
					}
			}
			else
				{
					document.forms.count.count2.value=+min+" : "+sec+" remaining"
				
					if (thesec==0) {
						themin=themin-1
						thesec=59
					}
					else
						thesec=thesec-1
				}
		thetimeout=setTimeout("countdown(themin,thesec)",1000)
		}

		</script>
	
</head>
<body onLoad="javascript:setInterval('xajax_receiveChat()',1333);">
<?php if(isset($_SESSION['permission'])) {



	$handle2 = fopen("games/bitenoyun".$_SESSION['permission'].".txt", "w");
	fwrite($handle2,"0 ");
	fclose($handle2);




echo <<<HERE5
<div name="gametime" id="gametime">
<form name="count">
<input type="text" size="20" name="count2">
</form>



<script>


HERE5;

include 'conf.php';


	$handle2 = fopen("games/current".$_SESSION['permission'].".txt", "r");
	$fileinfo=fscanf($handle2, "%s %d %d %d %s %d %s %s");
	list ($room_name,$row_num,$col_num,$unique_words,$duration,$in_game,$rem_time,$letters) = $fileinfo;	
	fclose($handle2);


$countdown=0;
$count_down=str2int($rem_time{2})+str2int($rem_time{1})*10+str2int($rem_time{0})*100;
$duration_t=str2int($duration{2})+str2int($duration{1})*10+str2int($duratin{0})*100;

$kalan_sure=$duration_t-$count_down;

$ilk=$kalan_sure%60;
$ikinci=($kalan_sure-$ilk)/60;
echo "countdown(".$ikinci.",".$ilk.")";


echo <<< HERE
</script>
</div>
<div class="players" id="players" align="left" name="players">

</div>

<div name="playagain" id="playagain" class="playagain">
</div>

<div id="chatHeaders" name="chatHeaders" class="chatHeaders" align=left>
<form name="chatbuton" id="chatbuton" action="javascript:void(null);" onsubmit="submitChat();" align=left>

<select name="smenu" id="smenu" align=left>
<option value="0" selected>To ALL</option>
HERE;

	$handle2 = fopen("games/current".$_SESSION['permission'].".txt", "r");
	$fileinfo=fscanf($handle2, "%s");
	list ($room_name) = $fileinfo;	
	fclose($handle2);
	
	$room_name_len=strlen($room_name)-1;
	$gonderilecek="K000".$room_name_len.$room_name;
	
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
	
	$active_users_len=str2int($gelen{1});
	
	
	
	$active_users=0;
	for ($c=0;$c<$active_users_len;$c++) {
		$active_users=$active_users+str2int($gelen{2+$c})*usal(10,$active_users_len-$c-1);
	}
	
	$nerdeyim=2+$active_users_len;
	$c=0;
	while ($c<$active_users) {
	
	$pname_len=$gelen{$nerdeyim+3};
	$id=str2int($gelen{$nerdeyim})*100+str2int($gelen{$nerdeyim+1})*10+str2int($gelen{$nerdeyim+2});
	$pname=substr($gelen,$nerdeyim+4,$pname+1);
	
	echo "<option value=\"".$id."\">".$pname."</option>";
	
	$nerdeyim=$nerdeyim+5+$pname_len;
	$c++;
	
	}



echo <<< HERE2
</select>
<br><hr>
<input style="font-size: 16px; font-family: Arial;" name="chatMsg" id="chatMsg" size="10" value="" align=left><input id="submitButton2" type="submit" value="Send Chat Message"/>

</form>
</div>

<div class="chat" id="chat" align="left" name="chat">

</div>

HERE2;
}
?>
</body>
</html>