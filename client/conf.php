<?php

$service_port = 9000;
$address = "144.122.238.163";
$packet_size=2048;

$dbhost='db4free.net';
$dbuser='';
$dbpass='';
$db='adminpanel';

function str2int($str) {
	switch ($str) {
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;	
		case '5':
			return 5;	
		case '6':
			return 6;
		case '7':
			return 7;	
		case '8':
			return 8;	
		case '9':
			return 9;
	}

}


function usal($x,$y) {
	$s=0;
	$toplam=1;
	while ($s<$y) {
	$toplam=$toplam*$x;
	$s++;
	}
return $toplam;
}


function drawBoard($rowN, $colN, $board)
{
	for($i=0; $i<$rowN * $colN; $i++)
	{
		if($i%$colN == 0)
		echo '<br>';
	
		echo $board[$i];
	}
	
}

function array_remval($val,&$arr)
{
  $i=array_search($val,$arr);
  if($i===false)return false;
  $arr=array_merge(array_slice($arr, 0,$i), array_slice($arr, $i+1));
  return true;
}


function getTrace($m, $n, $board, $word)
{
/*
	drawBoard($m, $n, $board);

	echo '<br><br><br><br>';
*/
	$wordLength = strlen($word);

	if($wordLength < 1)
		return NULL;

	$turn = 0;
	$candidateCount = 0;

for($i=0;$i<$m;$i++)
     for($j=0;$j<$n;$j++)
      if($word[0] == $board[$i*$n+$j])
	{
          for($k=0;$k<$m*$n;$k++) $mark[$turn][$candidateCount][$k] = 0;
          $mark[$turn][$candidateCount][$i*$n+$j] = 1;
          $last[$turn][$candidateCount][0] = $i;
          $last[$turn][$candidateCount][1] = $j;
          $candidateCount++;
	}
	

    for($i=1;$i<$wordLength;$i++)
    {

        $newCandidateCount = 0;
        for($j=0;$j<$candidateCount;$j++)
         for($a=-1;$a<=1;$a++)
          for($b=-1;$b<=1;$b++)
           {
              $y = $last[$turn][$j][0];
              $x = $last[$turn][$j][1];

              $pos = ($y+$a)*$n + ($x+$b);

		if(($y+$a>=0) && ($y+$a<$m) && ($x+$b>=0) && ($x+$b<$n))
		{

		if($word[$i] == $board[$pos])
		{

                if($mark[$turn][$j][$pos] == 0)
		{
		
			for($k=0;$k<$m*$n;$k++)
			{
				if($turn == 0)
				$myTurn = 1;
				else
				$myTurn = 0;
				
				$mark[$myTurn][$newCandidateCount][$k] = $mark[$turn][$j][$k];
			}

			if($turn == 0)
			$myTurn = 1;
			else
			$myTurn = 0;
			
			$mark[$myTurn][$newCandidateCount][$pos] = $i+1;
			$last[$myTurn][$newCandidateCount][0] = $y+$a;
			$last[$myTurn][$newCandidateCount][1] = $x+$b;
			$newCandidateCount++;
		}
}}}


	if($turn == 0)
	$turn = 1;
	else
	$turn = 0;

        $candidateCount = $newCandidateCount;
}

if($candidateCount < 1) return NULL;
    return($mark[$turn][0]);
}


?>
