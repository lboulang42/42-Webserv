<!DOCTYPE html>
<html>

<head>
    <title>Test DELETE</title>
</head>
<body>
    <h1>Test DELETE</h1>
    <?php
    if ($_SERVER['REQUEST_METHOD'] == 'DELETE') {
        echo "<h2>DELETE Parameters:</h2>";
        parse_str(file_get_contents("php://input"), $delete_vars);
        foreach ($delete_vars as $key => $value) {
            echo "<p>$key: $value</p>";
        }
    } else {
        echo "<p>Please send a DELETE request.</p>";
        http_response_code(405);
    }
    ?>
?>
</html>