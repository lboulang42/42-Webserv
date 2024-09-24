<?php
// Print GET parameters
echo "GET parameters:<br>";
foreach ($_GET as $key => $value) {
    echo "$key: $value<br>";
}

// Print POST parameters
echo "<br>POST parameters:<br>";
foreach ($_POST as $key => $value) {
    echo "$key: $value<br>";
}
?>