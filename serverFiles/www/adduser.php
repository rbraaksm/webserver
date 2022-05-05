<?php
parse_str($_SERVER['QUERY_STRING'], $output);
?>
<html>
<body>
<?php
echo "<h2>input1: " . $output['input1'] . "</h2></br>";
echo "<h2>input2: " . $output['input2'] . "</h2></br>";
?>
</body>
</html>
