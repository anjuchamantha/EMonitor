#include "ESP32_MailClient.h"
#include "config.h"

SMTPData smtpData;

// Callback function to get the Email sending status
void sendCallback(SendStatus info);

void sendMail(double temperature)
{
    Serial.println("\n----------------");
    Serial.println("[MAIL] Preparing to send email...");
    Serial.println();

    smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);
    smtpData.setSender("EMonitor Device", emailSenderAccount);
    smtpData.setPriority("High");
    smtpData.setSubject(emailSubject);
    smtpData.setMessage("<div><h1 style=\"color:#ff9966;\"> EMonitor Device Alert !</h1><p>Temperature threshold exceeded !</p><h2 style=\"color:#cc3300;\"> Temperature = " + String(temperature) + " °C</h2><p><a href=\"" + String(serverBase) + "\">Visit EMonitor website</a></p></div>",
                        true);
    smtpData.addRecipient(emailRecipient);
    //smtpData.addRecipient("YOUR_OTHER_RECIPIENT_EMAIL_ADDRESS@EXAMPLE.com");
    smtpData.setSendCallback(sendCallback);

    if (!MailClient.sendMail(smtpData))
        Serial.println("[MAIL] Error sending Email, " + MailClient.smtpErrorReason());

    smtpData.empty();
}

// Callback function to get the Email sending status
void sendCallback(SendStatus msg)
{
    Serial.println("[MAIL]" + msg.info());
    if (msg.success())
    {
        Serial.println("----------------");
    }
}