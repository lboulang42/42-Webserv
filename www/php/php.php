<!DOCTYPE html>
<html>
<head>
    <title>PHP Parameter Echo</title>
</head>
<body>
    <?php
    // Echo GET parameters
    if (!empty($_GET)) {
        echo "<h2>GET Parameters:</h2>";
        foreach ($_GET as $key => $value) {
            echo "<p>$key: $value</p>";
        }
    }

    // Echo POST parameters
    if (!empty($_POST)) {
        echo "<h2>POST Parameters:</h2>";
        foreach ($_POST as $key => $value) {
            echo "<p>$key: $value</p>";
        }
    } else {
        echo "<p>No POST parameters found.</p>";
    }
    ?>
</body>
</html>