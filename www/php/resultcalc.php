<!DOCTYPE html>
<html>
<head>
    <title>Calculation results</title>
</head>
<body>
    <h1>Calculation results</h1>
    <?php
        if (isset($_POST["x"]) && isset($_POST["y"])) {
            $x = $_POST["x"];
            $y = $_POST["y"];

            if (!empty($x) && !empty($y)) {
                $result = $x + $y;
                echo "<p>Result: $result</p>";
            } else {
                echo "<p>Please send me x and y</p>";
            }
        }
        else {
            echo "<p>No x or y provided</p>";
        }
    ?>
</body>
</html>
