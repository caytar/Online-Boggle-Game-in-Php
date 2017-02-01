<?php
require ('./lib/xajax.inc.php');
session_start();



function boarduCiz($formVars) {
	include 'conf.php';
// 	session_start();
	$typed_letters=trim($formVars['word']);


	$gonderilecek= "<table border=1 cellspacing=0 cellpadding=0>";

	$handle2 = fopen("games/current".$_SESSION['permission'].".txt", "r");
	$fileinfo=fscanf($handle2, "%s %d %d %d %s %d %s %s");
	list ($room_name,$row_num,$col_num,$unique_words,$duration,$in_game,$rem_time,$letters) = $fileinfo;	
	fclose($handle2);


	$result = getTrace($row_num,$col_num,$letters,$typed_letters);

	$i=0;
	while ($i<$row_num) {
		$gonderilecek=$gonderilecek."<tr>";
		$j=0;
		while($j<$col_num) {
			if ($result[$i*$row_num+$j]==0)
				$gonderilecek=$gonderilecek."<td width=40><div align=center style=\"font-weight: bold; font-size: 32px;\"> ".$letters{$i*$col_num+$j}." </div></td>";
			else	{
				$flag=1;
				$yazi3=10-$result[$i*$row_num+$j];
				$gonderilecek=$gonderilecek."<td bgcolor=\"#e".$yazi3.$yazi3."f".$yazi3."f"."\" width=40><div align=center style=\"font-weight: bold; font-size: 32px;\"> ".$letters{$i*$col_num+$j}." </div></td>";	
				}
					
			$j++;
		}
		$gonderilecek=$gonderilecek."</tr>";
		$i++;
	}



	$gonderilecek=$gonderilecek."</table>";



	$objResponse = new xajaxResponse();
   	$objResponse->addAssign("board","innerHTML",$gonderilecek);
	return $objResponse;


}

function guessWord($state,$formVars) {
	include 'conf.php';
	/*session_start()*/;
	$user=$_SESSION['permission'];

	$filename="files/".$user.".txt";

	if (strlen($user)==1) {
		$user="00".$user;
		}
	else if (strlen($user)==2) {
			$user="0".$user;
			}

	$handle2 = fopen("games/bitenoyun".$_SESSION['permission'].".txt", "r");
	$fileinfo=fscanf($handle2, "%d");
	list ($bitti_flag) = $fileinfo;	
	fclose($handle2);

if ($state==1 && $bitti_flag==0) {
		$handle2 = fopen("games/current".$_SESSION['permission'].".txt", "r");
		$fileinfo=fscanf($handle2, "%s %d %d %s %s %s %s %s");
		list ($room_name,$row_num,$col_num,$asd3,$asd4,$asd5,$asd6,$letters) = $fileinfo;	
		fclose($handle2);
	
		$kelime=trim($formVars['word']);
		$wordlen=strlen($kelime)-1;
		$room_len=strlen($room_name)-1;
		$gonderilecek="B".$user.$wordlen.$kelime.$room_len.$room_name;
	
		$result = getTrace($row_num,$col_num,$letters,$kelime);
		$flag=0;
		for ($m=0;$m<$row_num;$m++)
			for ($n=0;$n<$col_num;$n++)
				if ($result[$m*$row_num+$n] == 1) $flag=1;

		if ($flag==1) {
			// Create a TCP/IP socket. 
			$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
			if ($socket === false)
			echo "socket_create() failed. ";
		
			$result = socket_connect($socket, $address, $service_port);
			if ($result === false)
			echo "socket_connect() failed.";
		
			$word_control_result = '';
		
			socket_write($socket, $gonderilecek, strlen($gonderilecek));	
		
			while ($word_control_result = socket_read($socket, $packet_size)) {
				break;
				}
		
			socket_close($socket);
			
			////////////////////////
			
			$yazdirilacak=$kelime." <i>(";
			
			if (substr($word_control_result,6+$wordlen,1)=="0") {
				$yazdirilacak=$yazdirilacak."Not in dictionary)</i>";
			}
			else {
				if (substr($word_control_result,6+$wordlen,1)=="2") {
					$yazdirilacak=$yazdirilacak."Already Guessed)</i>";
				}
				else {
// 					echo "Points".substr($word_control_result,8+$wordlen,1)."-----";
					if (substr($word_control_result,7+$wordlen,1)=="0") 
						$yazdirilacak=$yazdirilacak.substr($word_control_result,8+$wordlen,1)." points)</i>";
		
					else
						$yazdirilacak=$yazdirilacak.substr($word_control_result,7+$wordlen,2)." points)</i>";
				}
			}
		
		}
		else 	{
		
			$yazdirilacak=$kelime." <i>(Not in board!)</i>";
		}
	
	
		$handle2 = fopen($filename, "r");
		$dosyadan=fread($handle2, filesize($filename));
		fclose($handle2);
	
		$dosyadan=$yazdirilacak.'<br>'.$dosyadan;
		
		if ($flag == 1) {
			$handle = fopen("games/puan".$_SESSION['permission'].".txt", "w");

			if (substr($word_control_result,9+$wordlen,1)=="0")  {
				if (substr($word_control_result,10+$wordlen,1)=="0") {
					$dosyadan2="<font color=blue><center>Total: ".substr($word_control_result,11+$wordlen,1)." points</center></font><br>".$dosyadan;
					fwrite($handle, substr($word_control_result,11+$wordlen,1));
				}	
				else	{
					$dosyadan2="<font color=blue><center>Total: ".substr($word_control_result,10+$wordlen,2)." points</center></font><br>".$dosyadan;
					fwrite($handle, substr($word_control_result,10+$wordlen,2));
				}
			}
			else	{
				$dosyadan2="<font color=blue><center>Total: ".substr($word_control_result,9+$wordlen,3)." points</center></font><br>".$dosyadan;	
				fwrite($handle, substr($word_control_result,9+$wordlen,3));
			}

		
		fclose($handle);
		
		}
		else {
			$handle2 = fopen("games/puan".$_SESSION['permission'].".txt", "r");
			$toplam_puan=fread($handle2, filesize($filename));
			fclose($handle2);

			$dosyadan2="<font color=blue><center>Total: ".$toplam_puan." points</center></font><br>".$dosyadan;	
			
		}
	
	
		
		$handle = fopen($filename, "w");
		fwrite($handle, $dosyadan);
		fclose($handle);
	
	
	
	
		$gonderilecek= "<table border=1 cellspacing=0 cellpadding=0>";
	
		$handle2 = fopen("games/current".$_SESSION['permission'].".txt", "r");
		$fileinfo=fscanf($handle2, "%s %d %d %d %s %d %s %s");
		list ($room_name,$row_num,$col_num,$unique_words,$duration,$in_game,$rem_time,$letters) = $fileinfo;	
		fclose($handle2);
	
	
	
		$i=0;
		while ($i<$row_num) {
			$gonderilecek=$gonderilecek."<tr>";
			$j=0;
			while($j<$col_num) {
				$gonderilecek=$gonderilecek."<td width=40><div align=center style=\"font-weight: bold; font-size: 32px;\"> ".$letters{$i*$col_num+$j}." </div></td>";
			
				$j++;
			}
			$gonderilecek=$gonderilecek."</tr>";
			$i++;
		}
	
	
	
		$gonderilecek=$gonderilecek."</table>";

}
else 	{
	$handle2 = fopen($filename, "r");
	$dosyadan2=fread($handle2, filesize($filename));
	fclose($handle2);

	$gonderilecek= "<table border=1 cellspacing=0 cellpadding=0>";

	$handle2 = fopen("games/current".$_SESSION['permission'].".txt", "r");
	$fileinfo=fscanf($handle2, "%s %d %d %d %s %d %s %s");
	list ($room_name,$row_num,$col_num,$unique_words,$duration,$in_game,$rem_time,$letters) = $fileinfo;	
	fclose($handle2);



	$i=0;
	while ($i<$row_num) {
		$gonderilecek=$gonderilecek."<tr>";
		$j=0;
		while($j<$col_num) {
			$gonderilecek=$gonderilecek."<td width=40><div align=center style=\"font-weight: bold; font-size: 32px;\"> ".$letters{$i*$col_num+$j}." </div></td>";
		
			$j++;
		}
		$gonderilecek=$gonderilecek."</tr>";
		$i++;
	}



	$gonderilecek=$gonderilecek."</table>";

}

	$objResponse = new xajaxResponse();
	$objResponse->addAssign("scroll","innerHTML",$dosyadan2);

if ($state==0) {
	if ($bitti_flag==1)
		$objResponse->addAssign("submitButton","disabled","true");
	
}

if ($bitti_flag==1) {
	$objResponse->addAssign("submitButton","disabled","true");

}

	$objResponse->addAssign("word","value","");
   	$objResponse->addAssign("board","innerHTML",$gonderilecek);
	return $objResponse;
}

$xajax = new xajax("playing.php"); 
$xajax->registerFunction("guessWord");
$xajax->registerFunction("boarduCiz");
$xajax->processRequests();
?>



<html>
<head>
<?php $xajax->printJavascript('./'); ?>
	<link rel="stylesheet" href="css/main.css" type="text/css" media="screen" />
	<script type="text/javascript">
		function sf(){document.search.word.focus();}	
		function submitGuess(myword)
		{
			
			var isvalid
			isvalid=myword.length
			if (isvalid < 3) { return false;}
			else { xajax_guessWord(1,xajax.getFormValues("search"));}
			return false;
		}
		function boardDegisti(myword)
		{
			var isvalid
			isvalid=myword.length
			if (isvalid == 0) { return false;}
			else { xajax_boarduCiz(xajax.getFormValues("search"));}
			
		}

	</script>
	
</head>
<body onload="sf(); xajax_guessWord(0,0)">
<?php 
if(isset($_SESSION['permission'])) 
{
	$userid=$_SESSION['permission'];

	$handle2 = fopen("history2/".$_SESSION['permission'].".txt", "r");
	$fileinfo=fscanf($handle2, "%d");
	if (isset($fileinfo)) list ($sayi) = $fileinfo;	
	fclose($handle2);

	if ($sayi==999)
	{
	


		echo "<div id=\"board\" name=\"board\" class=\"board\"><table border=1 cellspacing=0 cellpadding=0>";


				$handle2 = fopen("games/current".$_SESSION['permission'].".txt", "r");
				$fileinfo=fscanf($handle2, "%d %s %d %d %d %s %d %s %s");
				list ($room_name,$row_num,$col_num,$unique_words,$duration,$in_game,$rem_time,$letters) = $fileinfo;	
				fclose($handle2);

		
		$i=0;
		while ($i<$row_num) {
			echo "<tr>";
			$j=0;
			while($j<$col_num) {
				echo "<td width=40><div align=center style=\"font-weight: bold; font-size: 32px;\"> ".$letters{$i*$col_num+$j}." </div></td>";
				$j++;
			}
			echo "</tr>";
			$i++;
		}
		
		
		echo "</table></div><div class=\"scroll\" id=\"scroll\" align=\"left\" name=\"scroll\"></div>";
		
		echo "<script>xajax_guessWord(0,0);</script>";	

	}
	
	else 	{

		echo "<div id=\"deneem\" name=\"deneem\"></div><div id=\"board\" name=\"board\" class=\"board\"><table border=1 cellspacing=0 cellpadding=0>";
		
				$handle2 = fopen("games/current".$_SESSION['permission'].".txt", "r");
				$fileinfo=fscanf($handle2, "%s %d %d %d %s %d %s %s");
				list ($room_name,$row_num,$col_num,$unique_words,$duration,$in_game,$rem_time,$letters) = $fileinfo;	
				fclose($handle2);
		
		$i=0;
		while ($i<$row_num) {
			echo "<tr>";
			$j=0;
			while($j<$col_num) {
				echo "<td width=40><div align=center style=\"font-weight: bold; font-size: 32px;\"> ".$letters{$i*$col_num+$j}." </div></td>";
				$j++;
			}
			echo "</tr>";
			$i++;
		}
		
		
		echo "</table></div><div id=\"gonderbutonu\" name=\"gonderbutonu\" class=\"gonderbutonu\"><form name=\"search\" id=\"search\" action=\"javascript:void(null);\" >
		<input style=\"font-size: 16px; font-family: Arial;\" name=\"word\" id=\"word\" size=\"10\" value=\"\" maxlength=\"10\" onkeyup=\" boardDegisti(document.getElementById('word').value)\">
		<input id=\"submitButton\" type=\"submit\" value=\"Guess\" name=\"submitButton\" onclick=\"submitGuess(document.getElementById('word').value);\" />
		</form></div><div class=\"scroll\" id=\"scroll\" align=\"left\" name=\"scroll\"></div>";


	}

}
else echo "You are not logged in";
?>
</body>
</html>