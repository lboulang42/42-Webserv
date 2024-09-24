

<?php

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    if (isset($_FILES['file']) && $_FILES['file']['error'] == 0) {
        if (!isset($_SERVER['UPLOAD_DIR'])) {
            echo "UPLOAD_DIR is not set.\n";
            exit;
        }
        if (is_uploaded_file($_FILES['file']['tmp_name'])) {
            echo "File " . $_FILES['file']['name'] . " uploaded successfully.\n";
        } else {
            echo "File " . $_FILES['file']['name'] . " not uploaded.\n";
        }
        $upload_dir = getenv('UPLOAD_DIR');
        $real_upload_dir = realpath($upload_dir);
        $target_file = $real_upload_dir . '/' . basename($_FILES["file"]["name"]);
        echo "real_upload_dir is " . $real_upload_dir . "\n";
        // $upload_dir = '../../var/uploadsdir/';
        // $uploaded_file_path = $upload_dir . basename($_FILES['file']['name']);

        if (isset($_SERVER['PATH_INFO'])) {
            echo $_SERVER['PATH_INFO'];
        } else {
            echo "PATH_INFO is not set.\n";
        }
        echo "trying to send to " . $target_file . "\n";

        if (move_uploaded_file($_FILES['file']['tmp_name'], $target_file)) {
            echo "File Uploaded To " . $target_file .  "\n";
            // header("Location: /" . $target_file);
            exit;
        } else {
            echo "An error occurred while uploading the file.\n";
        }
    } else {
        echo "No file was uploaded, or there was an error with the upload.\n";
        if (isset($_FILES['file'])) {
            echo "Error code: " . $_FILES['file']['error'] . "\n";
        } else {
            echo "No Error code provided.\n";
        }
    }
}
?>
