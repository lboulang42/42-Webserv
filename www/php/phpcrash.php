<!DOCTYPE html>
<html>
<head>
    <title>PHP Crash Example</title>
</head>
<body>
    <h1>PHP Crash Example</h1>

    <?php
        // This PHP script will cause a crash
        $crash = null;
        echo $crash->method();
</body>
</html>