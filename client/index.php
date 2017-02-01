<?php

require ('./lib/xajax.inc.php');
session_start();


function loadHistory($which) {
	include('conf.php');
	/*session_start()*/;

	$filename="history/".$_SESSION['permission'].$which.".txt";
	$filename2="history2/".$_SESSION['permission'].".txt";

	if (isset($_SESSION['permission'])) {
		$user=$_SESSION['permission'];
// 		echo "----".$filename;
		$handle2 = fopen($filename, "r");
		$fileinfo=fscanf($handle2, "%s %s %d %d %s %d %s");
		list ($userid,$room_name,$row_sayisi,$col_sayisi,$tahta,$puan,$guessed_words) = $fileinfo;	
		fclose($handle2);

		$handle3 = fopen("history2/".$_SESSION['permission'].".txt", "w");
		fwrite($handle3,"999");
		fclose($handle3);


		$handle3 = fopen("files/".$user.".txt", "w");
		fwrite($handle3,"<font color=blue><center>Total: ".$puan."points</center></font><br>".$guessed_words);
		fclose($handle3);

		$handle = fopen("games/current".$_SESSION['permission'].".txt", "w");
		fwrite($handle,$room_name." ".$row_sayisi." ".$col_sayisi." 0  030 0 000 ".$tahta);
		fclose($handle);
// 		echo "---".$tahta;
		$text = "<iframe FRAMEBORDER=0 src =\"./playing.php\" width=\"100%\" marginheight=20 height=310></iframe>";

	}

	$objResponse = new xajaxResponse();
	$objResponse->addAssign("cont","innerHTML",$text);
	
	return $objResponse;

}


function whichRoom($which) {
	include('conf.php');
	/*session_start()*/;

	$filename="games/".$which.".txt";
	if (isset($_SESSION['permission'])) {
		$handle2 = fopen($filename, "r");
		$dosyadan=fread($handle2, filesize($filename));
		fclose($handle2);
		
		$handle = fopen("games/current".$_SESSION['permission'].".txt", "w");
		fwrite($handle,$dosyadan);
		fclose($handle);
		
		$handle3 = fopen("games/current".$_SESSION['permission'].".txt", "r");
		$fileinfo=fscanf($handle3,"%s");
		list ($room_name) = $fileinfo;		
		fclose($handle3);
		
		$userid=$_SESSION['permission'];
		if (strlen($_SESSION['permission'])==1) $userid="00".$userid;
		else if (strlen($_SESSION['permission'])==2) $userid="0".$userid;
		
		$gonderilecek="C".$userid.(strlen($room_name)-1).$room_name;
		
		$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		if ($socket === false)
			echo "socket_create() failed. ";

		$result = socket_connect($socket, $address, $service_port);
		if ($result === false)
			echo "socket_connect() failed.";
		
		socket_write($socket, $gonderilecek, strlen($gonderilecek));	
		
		$gelen = '';
		
		while ($gelen = socket_read($socket, 1024)) {
			break;
		}
		
		socket_close($socket);
		
		if ($gelen{4}==1) {
		
			$handle = fopen("files/".$_SESSION['permission'].".txt", "w");
			fwrite($handle," ");
			fclose($handle);

			$text = "<iframe FRAMEBORDER=0 src =\"./playing.php\" width=\"100%\" marginheight=20 height=310></iframe>
			<iframe FRAMEBORDER=0 src =\"./chat.php\" width=\"100%\" height=320></iframe>";

			
			
			$handle3 = fopen("games/current".$_SESSION['permission'].".txt", "r");
			$fileinfo=fscanf($handle3,"%s %d %d %d %s %d %s");
			list ($room_name,$row_num,$col_num,$unique_words,$duration,$in_game,$rem_time) = $fileinfo;		
			fclose($handle3);
			
			
			$dosyadan=$room_name." ".$row_num." ".$col_num." ".$unique_words." ".$duration." ".$in_game." ".substr($gelen,5,3)." ".substr($gelen,8,$row_num*$col_num);
			$handle = fopen("games/current".$_SESSION['permission'].".txt", "w");
			fwrite($handle,$dosyadan);
			fclose($handle);




		}
		else {
			$text="This room has not started yet. Please check in a few seconds.";
		} 
	}
	else 
		$text="You are not logged in.";
			
	$objResponse = new xajaxResponse();
	$objResponse->addAssign("cont","innerHTML",$text);
	
	return $objResponse;
		
}

function selectContent($content) {
	include('conf.php');

	if ($content==-1)
		$text = "<iframe FRAMEBORDER=0 src =\"./login.php\" width=\"100%\" marginheight=0 height=600></iframe>";
		
	if ($content==0)
		$text = "<iframe FRAMEBORDER=0 src =\"./about.php\" width=\"100%\" marginheight=0 height=600></iframe>";
	if ($content==1)
		$text = "<iframe FRAMEBORDER=0 src =\"./howto.php\" width=\"100%\" marginheight=0 height=600></iframe>";
	if ($content==2) 
		$text = "<iframe FRAMEBORDER=0 src =\"./features.php\" width=\"100%\" marginheight=0 height=600></iframe>";	
	
	if ($content==4) { // Fetch room list
		
// 		session_start();

		if (isset($_SESSION['permission'])) {
			$gonderilecek="D000";
					
			$handle3 = fopen("history2/".$_SESSION['permission'].".txt", "w");
			fwrite($handle3,"0");
			fclose($handle3);

			$handle3 = fopen("games/bitenoyun".$_SESSION['permission'].".txt", "w");
			fwrite($handle3,"0");
			fclose($handle3);

			$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
			if ($socket === false)
			echo "socket_create() failed. ";
	
			$result = socket_connect($socket, $address, $service_port);
			if ($result === false)
			echo "socket_connect() failed.";
			
			socket_write($socket, $gonderilecek, strlen($gonderilecek));	
			
			$gelen = '';
			
			while ($gelen = socket_read($socket, 1024)) {
				break;
			}
			
			socket_close($socket);
			
			$room_sayisi=str2int($gelen{2})+10*str2int($gelen{1});
			$i=0;
			$nerdeyiz=3;
			while ($i<$room_sayisi) {
				$odalar[$i]=str2int($gelen{$nerdeyiz});
				$nerdeyiz=$nerdeyiz+$odalar[$i]+12;
				$i++;
			}
			
			$i=0;
			$nerdeyiz=4;
			while ($i<$room_sayisi) {
				$oda_ozellikleri[$i][6]=substr($gelen,$nerdeyiz,$odalar[$i]+1); // Room adlari
				
				$nerdeyiz=$nerdeyiz+$odalar[$i]+1;
				$oda_ozellikleri[$i][0]=$gelen{$nerdeyiz}; // row sayisi
				$oda_ozellikleri[$i][1]=$gelen{$nerdeyiz+1}; // column sayisi
				$oda_ozellikleri[$i][2]=$gelen{$nerdeyiz+2}; // unique olsun mu
				$oda_ozellikleri[$i][3]=substr($gelen,$nerdeyiz+3,3); // oyun suresi
				$oda_ozellikleri[$i][4]=$gelen{$nerdeyiz+6}; // oyun aktif mi yoksa break mi
				$oda_ozellikleri[$i][5]=substr($gelen,$nerdeyiz+7,3); // oyun basliyali ne kadar zaman gecti
				$nerdeyiz=$nerdeyiz+11;
				$i++;
			}
			
			$i=0;
			while ($i<$room_sayisi) {
			
				$filename="games/".$i.".txt";
				$handle = fopen($filename, "w");
				fwrite($handle,$oda_ozellikleri[$i][6]." ".$oda_ozellikleri[$i][0]." ".$oda_ozellikleri[$i][1]." ".$oda_ozellikleri[$i][2]." ".$oda_ozellikleri[$i][3]." ".$oda_ozellikleri[$i][4]." ".$oda_ozellikleri[$i][5]);
				fclose($handle);	
				$i++;
			}
		
			$text = "<table border=1 align=center width=100%><tr><td><b>Room Name</b></td> <td><b>Dimensions</b></td> <td width=120><b>Only Unique Words Counted</b></td> <td><b>Time</b></td> <td width=40><b>Game is Active</b></td></tr>";
			
			$i=0;	
			while($i<$room_sayisi) {
				$str_game_duration=$oda_ozellikleri[$i][3];
				$str_remaining_time=$oda_ozellikleri[$i][5];
				$game_duration=str2int($str_game_duration{0})*100+str2int($str_game_duration{1})*10+str2int($str_game_duration{2});
				$remaining_time=str2int($str_remaining_time{0})*100+str2int($str_remaining_time{1})*10+str2int($str_remaining_time{2});
				
				$remaining_time2=$game_duration-$remaining_time;
				$remaining_time=15-$remaining_time;
	
				if ($oda_ozellikleri[$i][4]==1) {
					$yazi1="Reamining Time: ".$remaining_time2." sec";
					$yazi2="<b>".$oda_ozellikleri[$i][4]."</b>";
					}
				else 	{
					
					
					$yazi1="Game will start : ".$remaining_time." sec later";
					$yazi2=$oda_ozellikleri[$i][4];
					}
				$text=$text."<tr> <td><a href=\"javascript:void(null)\" onclick=\"xajax_whichRoom('$i')\">".$oda_ozellikleri[$i][6]."</a></td> <td>".$oda_ozellikleri[$i][0]."x".$oda_ozellikleri[$i][1]."</td> <td>".$oda_ozellikleri[$i][2]."</td> <td>Game Duration: ".$game_duration." sec<br>".$yazi1."</td> <td>".$yazi2."</td> </tr>";
				$i++;
			}
					
			$text=$text."</table>";
		}
		else  	{
			$text="First you should log in to system";
		}

	}
		
	if ($content==5) { // Log out
		/*session_start()*/;
		$id=$_SESSION['permission'];
		unset($_SESSION['permission']);
		$text = "<iframe FRAMEBORDER=0 src =\"./login.php\" width=\"90%\" marginheight=0 height=600></iframe>";	
	}
	if ($content==6) { // Previous Games

		/*session_start()*/;

		if (isset($_SESSION['permission'])) {
			$user=$_SESSION['permission'];
			if (strlen($user)==1) {
				$user="00".$user;
			}
			else 
				if (strlen($user)==2) {
					$user="0".$user;
				}

			$handle3 = fopen("history2/".$_SESSION['permission'].".txt","w");
			fwrite($handle3,"0");
			fclose($handle3);


			$gonderilecek="G".$user;
			
			$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
			if ($socket === false)
			echo "socket_create() failed. ";
	
			$result = socket_connect($socket, $address, $service_port);
			if ($result === false)
			echo "socket_connect() failed.";
			
			socket_write($socket, $gonderilecek, strlen($gonderilecek));	
			
			$gelen = '';

			while ($gelen = socket_read($socket, $packet_size)) {
				break;
			}

			socket_close($socket);

			$text = "<table border=1 align=center width=100%><tr><td><b>Room Name</b></td> <td><b>Dimensions</b></td> <td><b>Date of the Game</b></td></tr>";
			
			$log_sayisi_len=str2int($gelen{4});
			$log_sayisi=0;
			$w=0;
			while ($w<$log_sayisi_len) {
				$log_sayisi=$log_sayisi+str2int($gelen{5+$m})*usal(10,$log_sayisi_len-$w-1);
				$w++;
			}

			$nerdeyim=5+$log_sayisi_len;
			$w=0;
// 			echo "----".$log_sayisi;
			while ($w<$log_sayisi) {
				$room_name_len=str2int($gelen{$nerdeyim});
				$room_name=substr($gelen,$nerdeyim+1,$room_name_len+1);
				$nerdeyim=$nerdeyim+$room_name_len+2;
				$gtime_len=str2int($gelen{$nerdeyim})*10+str2int($gelen{$nerdeyim+1});
				$gtime=substr($gelen,$nerdeyim+2,$gtime_len);
				$nerdeyim=$nerdeyim+$gtime_len+2;
				$row_sayisi=str2int($gelen{$nerdeyim});
				$col_sayisi=str2int($gelen{$nerdeyim+1});
				$nerdeyim=$nerdeyim+2;
 				$tahta=substr($gelen,$nerdeyim,$row_sayisi*$col_sayisi);
				$nerdeyim=$nerdeyim+$row_sayisi*$col_sayisi;
				$puan_len=str2int($gelen{$nerdeyim});
				$puan=substr($gelen,$nerdeyim+1,$puan_len);
				$nerdeyim=$nerdeyim+$puan_len+1;
				$words_len_len=str2int($gelen{$nerdeyim});
				$words_len_str=substr($gelen,$nerdeyim+1,$words_len_len);
				$words_len=0;
				for ($c=0;$c<$words_len_len;$c++) 
					$words_len=$words_len+str2int($words_len_str{$c})*usal(10,$words_len_len-$c-1);
				$nerdeyim=$nerdeyim+$words_len_len+1;
				$words=substr($gelen,$nerdeyim,$words_len);
				$words2="";
				for ($c=0;$c<$words_len;$c++) {
					if ($words{$c}==" ") $words2=$words2."&nbsp;";
					else $words2=$words2.$words{$c};
				}

				$dosyaya_yaz=$user." ".$room_name." ".$row_sayisi." ".$col_sayisi." ".$tahta." ".$puan." ".$words2;
				$handle = fopen("history/".$_SESSION['permission'].$w.".txt", "w");
				fwrite($handle,$dosyaya_yaz);
				fclose($handle);
 				$nerdeyim=$nerdeyim+$words_len;

				$text=$text."<tr><td><a href=\"javascript:void(null)\" onclick=\"xajax_loadHistory(".$w.")\">".$room_name."</a></td><td>".$row_sayisi."x".$col_sayisi."</td><td>".$gtime."</td></tr>";
				//echo "Room name=".$room_name." Time Len=".$gtime_len." Date=".$gtime." RowxCol=".$row_sayisi.$col_sayisi." Board=".$tahta." Puan Len=".$puan_len." Puan=".$puan." Words-Len-Len=".$words_len_len." Words-Len-Str=".$words_len_str." Words-Len=".$words_len." WORDS=".$words2;
				$w++;
			}

					
			$text=$text."</table>";
		}
		else	{
			$text="You should log in to system. Because previous games will be fetched according to your user id.";
		}

	
		
	}
	if ($content==7) { // ranking 

		/*session_start()*/;

		if (isset($_SESSION['permission'])) {
			$user=$_SESSION['permission'];
			if (strlen($user)==1) {
				$user="00".$user;
			}
			else 
				if (strlen($user)==2) {
					$user="0".$user;
				}

//  			$gonderilecek="G002";


			$gonderilecek="L000";
			
			$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
			if ($socket === false)
			echo "socket_create() failed. ";
	
			$result = socket_connect($socket, $address, $service_port);
			if ($result === false)
			echo "socket_connect() failed.";
			
			socket_write($socket, $gonderilecek, strlen($gonderilecek));	
			
			$gelen = '';

			while ($gelen = socket_read($socket, $packet_size)) {
				break;
			}

			socket_close($socket);

			$text = "<table border=1 align=center width=100%><tr><td><b>No</b></td> <td><b>Player Name</b></td> <td><b>Points</b></td></tr>";
			
			$log_sayisi_len=str2int($gelen{1});
			$log_sayisi=0;
			$w=0;
			while ($w<$log_sayisi_len) {
				$log_sayisi=$log_sayisi+str2int($gelen{2+$w})*usal(10,$log_sayisi_len-$w-1);
				$w++;
			}

			$nerdeyim=2+$log_sayisi_len;
			$w=0;
// 			echo "----".$log_sayisi;
			while ($w<$log_sayisi) {
				$room_name_len=str2int($gelen{$nerdeyim});		// player name length - 1
				$room_name=substr($gelen,$nerdeyim+1,$room_name_len+1);
				$nerdeyim=$nerdeyim+$room_name_len+2;
				$points_len_e=str2int($gelen{$nerdeyim});		// points length
				$points_e=substr($gelen,$nerdeyim+1,$points_len_e);
				$nerdeyim=$nerdeyim+$points_len_e+1;
				$no = $w+1;
				$text=$text."<tr><td>".$no."</td><td>".$room_name."</td><td>".$points_e."</td></tr>";
				//echo "Room name=".$room_name." Time Len=".$gtime_len." Date=".$gtime." RowxCol=".$row_sayisi.$col_sayisi." Board=".$tahta." Puan Len=".$puan_len." Puan=".$puan." Words-Len-Len=".$words_len_len." Words-Len-Str=".$words_len_str." Words-Len=".$words_len." WORDS=".$words2;
				$w++;
			}

					
			$text=$text."</table>";
		}
		else	{
			$text="You should log in to system.";
		}

	
		
	}
	if ($content==8) { // create room

		/*session_start()*/;

		if (isset($_SESSION['permission'])) {
			$text = "<iframe FRAMEBORDER=0 src =\"./croom.php\" width=\"100%\" marginheight=0 height=600></iframe>";

		}
		else	{
			$text="You should log in to system.";
		}

	
		
	}

	
		
	$objResponse = new xajaxResponse();
	$objResponse->addAssign("cont","innerHTML",$text);
	
	return $objResponse;
}


$xajax = new xajax("index.php"); 

$xajax->registerFunction("selectContent");
$xajax->registerFunction("whichRoom");
$xajax->registerFunction("loadHistory");
$xajax->processRequests();
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
	<title>CENGboggle</title>
	<?php $xajax->printJavascript('./'); ?>
	<meta http-equiv="content-type" content="text/html; charset=ISO-8859-9" />
	<META HTTP-EQUIV="Author" CONTENT="Cem AYTAR , Semsi Cihan YuCEL , Mehmet Emre SAHIN , Zeynal ZEYNALOV">
	<meta name="Description" content="Online Boggle - a word game" />
	<meta name="Keywords" content="online , boggle , bogle , boogle , word , game , dictionary , multiplayer" />
	<link rel="stylesheet" href="css/main.css" type="text/css" media="screen" />
	<script>
		function sf(){document.search.word.focus();}
		function f(o){var code = event.keyCode; switch (code) {case 37:break;case 38:break; case 39:break; case 40:break;  default:if(o.value != o.value.toLowerCase()) {o.value=o.value.toLowerCase();}}}
	</script>	
</head>
<body>


<div id="inner-wrapper">


	<div id="content-wrapper">
<table border=0>
<tr><td>
		<div id="sidebar">
			<div>
				<br />
				<img src="images/cengboggle-logo.png" alt="CENGboggle" />
			</div>

			<ul class="side-nav">
				<li><a href="javascript:void(null)" onclick="xajax_selectContent(-1)">Login</a></li>
				<li><a href="javascript:void(null)" onclick="xajax_selectContent(0)">About Us</a></li>
				<li><a href="javascript:void(null)" onclick="xajax_selectContent(1)">How To Play?</a></li>
				<li><a href="javascript:void(null)" onclick="xajax_selectContent(2)">Features</a></li>
				<li><a href="javascript:void(null)" onclick="xajax_selectContent(4)">PLAY</a></li>
				<li><a href="javascript:void(null)" onclick="xajax_selectContent(6)">Previous Games</a></li>
				<li><a href="javascript:void(null)" onclick="xajax_selectContent(7)">Rankings</a></li>
				<li><a href="javascript:void(null)" onclick="xajax_selectContent(8)">Create Room</a></li>
				<li><a href="javascript:void(null)" onclick="xajax_selectContent(5)">Log Out</a></li>
				
				
			</ul>
		</div>
</td>
<td width=100% align=center>		
	<div id="cont" name="cont" align="center">
	<script type="text/javascript">
	xajax_selectContent(-1);
	</script>
	</div>		
</td></tr></table>		
	</div>
	
</div>



</body>
</html>