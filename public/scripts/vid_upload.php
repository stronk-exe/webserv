<?php
if ($_SERVER["REQUEST_METHOD"] === "POST") {
    // Check if the file was uploaded without errors
    if (isset($_FILES["video"]) && $_FILES["video"]["error"] === UPLOAD_ERR_OK) {
        $file = $_FILES["video"];

        // Specify the destination directory where the uploaded file will be saved
        $uploadDir = "uploads/";

        // Create the directory if it doesn't exist
        if (!is_dir($uploadDir)) {
            mkdir($uploadDir, 0755, true);
        }

        // Generate a unique filename for the uploaded video
        $filename = uniqid() . "_" . $file["name"];

        // Move the uploaded file to the destination directory
        $destination = $uploadDir . $filename;
        if (move_uploaded_file($file["tmp_name"], $destination)) {
            echo "File uploaded successfully!";
        } else {
            echo "Failed to move the uploaded file.";
        }
    } else {
        echo "Error uploading the file.";
    }
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Upload and Display Video</title>
</head>
<body>
    <form method="POST" enctype="multipart/form-data">
        <input type="file" name="video">
        <input type="submit" value="Upload">
    </form>

    <hr>

    <?php
    // Display the uploaded video if available
    $videoPath = isset($destination) ? $destination : null;
    if ($videoPath && file_exists($videoPath)) {
        echo '<h2>Uploaded Video</h2>';
        echo '<video controls>';
        echo '<source src="' . $videoPath . '" type="video/mp4">';
        echo 'Your browser does not support the video tag.';
        echo '</video>';
    }
    ?>
</body>
</html>