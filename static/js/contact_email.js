// Contact: Email
// author: Zhida Li
// last modified: September 27, 2023

// Initialize EmailJS and add your form submission handler

// Initialize EmailJS
emailjs.init("cj5hHtgmjaM3QC5JY");


// Add event listener for form submission
document.getElementById('emailForm').addEventListener('submit', function (event) {
    event.preventDefault();

    let user_email = event.target.user_email.value;

    let to_name = "User";

    emailjs.send("service_dk4c0kv", "template_yh10odp", {
        email: user_email,
        to_name: to_name,
        from_name: "ZL & CNL",
        message: "Please check out our latest news and updates at zhidali.me!"
    })
        .then(function (response) {
            console.log('Email sent successfully!', response);
            alert('Email sent successfully!');  // Show success message
        }, function (error) {
            console.log('Failed to send email:', error);
            alert('Failed to send email. Please try again later.');  // Show error message
        });
});
