<?php
$s = "XXX XXX Hello world,Καλημέρα κόσμε,コンニチハ,Привет мир,你好 世界";
$hw = "/(?:(\w+\s*\w*),?)/u";

	const tom_EOS = 0;
	const tom_Equal = 1;
	const tom_Bool = 2;
	const tom_Dot = 3;
	const tom_FloatExp = 4;
	const tom_FloatDot = 5;
	const tom_Float_E = 6;
	const tom_Integer = 7; //5
	const tom_Quote3 = 8;
	const tom_Quote1 = 9;
	const tom_Apost3 = 10;
	const tom_Apost1 = 11;
	const tom_NewLine = 12; // 10
	const tom_Space = 13;
	const tom_LSquare = 14; //12
	const tom_RSquare = 15;
	const tom_LCurly = 16;
	const tom_RCurly = 17;
	const tom_Comma = 18;
	
	const tom_BareKey = 19; // 18
	const tom_EscapedChar = 20;
	const tom_Escape = 21;
	const tom_NoEscape = 22;
	const tom_Hash = 23;
	const tom_LitString = 24;
	const tom_SpacedEqual = 25;
	const tom_CommentStuff = 26;
		const tom_DateTime = 27;
		const tom_Time = 28;
	const tom_HashComment = 29;
	const tom_AnyValue = 30;
	const tom_Dig_Dig = 31;
	const tom_No_0Digit = 32;
	const tom_AnyChar = 33;

	$exp = [
		tom_Bool => '(true|false)',
 		tom_DateTime => 
 		'(\d{4}-\d{2}-\d{2}((?:T|\s)\d{2}:\d{2}:\d{2}(\.\d{6})?(Z|-\d{2}:\d{2})?)?)',
		tom_Time => '(\\d{2}:\\d{2}:\\d{2}(\\.\\d{6})?)',
 		tom_FloatExp => '([+-]?((\d_?)+([\.](\d_?)*)?)([eE][+-]?(_?\d_?)+))',
		tom_FloatExp => '([+-]?((\d_?)+([\.](\d_?)*)?)([eE][+-]?(_?\d_?)+))',
		tom_FloatDot => '([+-]?((\d_?)+([\.](\d_?)*)))',
		tom_Integer => '([+-]?(\d_?)+)',

		tom_Quote3 => '(\"\"\")',
		tom_Apost3 => '(\'\'\')',
		tom_Space => '(\h+)',
		tom_BareKey => '([-A-Z_a-z0-9]+)',

		tom_EscapedChar => '(\\(n|t|r|f|b|\\"|\\|u[0-9A-Fa-f]{4,4}|U[0-9A-Fa-f]{8,8})',
		tom_NoEscape => '([^\x{0}-\x{19}\x{22}\x{5C}]+)',
		tom_LitString => '([^\x{0}-\x{19}\x{27}]+)',

		tom_AnyValue => '([^\s\]\},]+)',
		tom_SpacedEqual => '(\h*=\h*)',
		tom_CommentStuff => '(\V*)',
		tom_HashComment => '(\h*#\V*|\h*)',

		tom_Dig_Dig => '([^\d]_[^\d])|(_\$)',
		tom_No_0Digit => '(0\d+)',
		tom_Float_E => '([^\d]_[^\d])|_[eE]|[eE]_|(_$)',
];

$name = [
		tom_Bool => 'tom_Bool',
 		tom_DateTime => 'tom_DateTime',
 		tom_Time => 'tom_Time',
 		tom_FloatExp => 'tom_FloatExp',
		tom_FloatExp => 'tom_FloatExp',
		tom_FloatDot => 'tom_FloatDot',
		tom_Integer => 'tom_Integer',

		tom_Quote3 => 'tom_Quote3',
		tom_Apost3 => 'tom_Apost3',
		tom_Space => 'tom_Space',
		tom_BareKey => 'tom_BareKey',

		tom_EscapedChar => 'tom_EscapedChar',
		tom_NoEscape => 'tom_NoEscape',
		tom_LitString => 'tom_LitString',

		tom_AnyValue => 'tom_AnyValue',
		tom_SpacedEqual => 'tom_SpacedEqual',
		tom_CommentStuff => 'tom_CommentStuff',
		tom_HashComment => 'tom_HashComment',

		tom_Dig_Dig => 'tom_Dig_Dig',
		tom_No_0Digit => 'tom_No_0Digit',
		tom_Float_E => 'tom_Float_E',
];



$test = [
	'1234',
	' = ',
	'92.501',
	'9.2501e-1',
	'\t\n\b\\',
	"'''",
	'2001-11-07',
	'2001-11-07 13:55:01',
	'2001-11-07 13:55:01',
	'2001-11-07 13:55:01.123456',
		
	'07:32:00',
	'00:32:00.999999',
	'Making big time',
	'#Regular expressions can be tricky',
	'a_b',
	'01',
	'.123',
	'Καλημέρα κόσμε,コンニチハ',
	'true',
	'false'
];

$len = strlen($s);
$offset = 8;
while($offset < $len) {
	$match = null;

	$result = preg_match($hw, $s, $match, 0, $offset);
	if (empty($match)) {
		break;
	}
	$capture = $match[1];
	$offset += strlen($match[0]);
	echo $capture . PHP_EOL;
}

$prefix = 'X 你好 世界 X';
$offset = strlen($prefix);

$wexp = [];
foreach($exp as $key => $val) {
	$wexp[$key] = "/" . $val . "/u";
}
foreach($test as $item) {
	// find matching expression
	// 
	$pf = $prefix . $item;
	echo '==== ' . $item . ' ====' . PHP_EOL;
	$match = null;
	foreach($wexp as $id => $exfind) {
		try {
			$result = preg_match($exfind . 'u', $pf, $match, 0, $offset);
		if (!empty($result)) {
			echo "  " . $match[1] . '   ** ' . $name[$id] . PHP_EOL;
		}
	}
	catch(\Exception $ex ) {
		echo "Damn " . $ex->getMessage() . PHP_EOL;
	}
	}
}

?>