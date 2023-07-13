<?php

$uploadDir = "uploads/"; // Move the variable declaration outside the conditional block

if ($_SERVER['REQUEST_METHOD'] === 'POST')
{
    // Handle file upload
    if (isset($_FILES["file"]) && $_FILES["file"]["error"] === UPLOAD_ERR_OK) {
        $file = $_FILES["file"];
        $fileName = basename($file["name"]);
        $targetPath = $uploadDir . $fileName;
        
        if (move_uploaded_file($file["tmp_name"], $targetPath)) {
            echo "File uploaded successfully.";
        } else {
            echo "Error uploading file.";
        }
    }
}

// Handle file deletion

else if (isset($_GET["delete"])) {
    $fileName = $_GET["delete"];
    $targetPath = $uploadDir . $fileName;
    
    if (file_exists($targetPath)) {
        if (unlink($targetPath)) {
            echo "File deleted successfully.";
        } else {
            echo "Error deleting file.";
        }
    } else {
        echo "File not found.";
    }
}

?>

<!-- HTML form to upload files -->
<form action="" method="POST" enctype="multipart/form-data">
    <input type="file" name="file">
    <input type="submit" value="Upload">
</form>

<!-- Display uploaded files -->
<?php
$files = array_diff(scandir($uploadDir), array('.', '..'));

foreach ($files as $file) {
    echo "<p>$file <a href=\"?delete=$file\">Delete</a></p>";
}
?>
