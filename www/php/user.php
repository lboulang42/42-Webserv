<!DOCTYPE html>
<html>
<head>
    <title>Login Page</title>
</head>
<body>
    <h1>Login Page</h1>
    <?php
        echo "<h2>GET Parameters:</h2>";
        foreach ($_GET as $key => $value) {
            echo "<p>$key: $value</p>";
        }

        echo "<h2>POST Parameters:</h2>";
        foreach ($_POST as $key => $value) {
            echo "<p>$key: $value</p>";
        }
    ?>
</body>
</html>
