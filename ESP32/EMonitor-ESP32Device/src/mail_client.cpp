#include "ESP32_MailClient.h"
#include "config.h"

// The Email Sending data object contains config and data to send
SMTPData smtpData;

// Callback function to get the Email sending status
void sendCallback(SendStatus info);

void sendMail(double temperature)
{
    Serial.println("\n----------------");
    Serial.println("[MAIL] Preparing to send email...");
    Serial.println();

    // Set the SMTP Server Email host, port, account and password
    smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);

    // For library version 1.2.0 and later which STARTTLS protocol was supported,the STARTTLS will be
    // enabled automatically when port 587 was used, or enable it manually using setSTARTTLS function.
    //smtpData.setSTARTTLS(true);

    // Set the sender name and Email
    smtpData.setSender("EMonitor Device", emailSenderAccount);

    // Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)
    smtpData.setPriority("High");

    // Set the subject
    smtpData.setSubject(emailSubject);

    // Set the message with HTML format
    smtpData.setMessage("<div><h1 style=\"color:#ff9966;\"> EMonitor Device Alert !</h1><p>Temperature threshold exceeded !</p><h2 style=\"color:#cc3300;\"> Temperature = " + String(temperature) + " °C</h2><p><a href=\"" + String(serverBase) + "\">Visit EMonitor website</a></p></div>",
                        true);
    // Set the email message in text format (raw)
    //smtpData.setMessage("Hello World! - Sent from ESP32 board", false);

    // Add recipients, you can add more than one recipient
    smtpData.addRecipient(emailRecipient);
    //smtpData.addRecipient("YOUR_OTHER_RECIPIENT_EMAIL_ADDRESS@EXAMPLE.com");

    smtpData.setSendCallback(sendCallback);

    //Start sending Email, can be set callback function to track the status
    if (!MailClient.sendMail(smtpData))
        Serial.println("[MAIL] Error sending Email, " + MailClient.smtpErrorReason());

    //Clear all data from Email object to free memory
    smtpData.empty();
}

// Callback function to get the Email sending status
void sendCallback(SendStatus msg)
{
    // Print the current status
    Serial.println("[MAIL]" + msg.info());

    // Do something when complete
    if (msg.success())
    {
        Serial.println("----------------");
    }
}