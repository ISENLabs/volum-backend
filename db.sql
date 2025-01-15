/* MariaDB */
/* This is just a summary of needed table for public use. */
CREATE TABLE volum_status (
    `id` INT AUTO_INCREMENT PRIMARY KEY,
    `status` VARCHAR(20) NOT NULL,
    `version` VARCHAR(20) NOT NULL
);

CREATE TABLE volum_vms (
    `id` INT AUTO_INCREMENT PRIMARY KEY,
    `ctid` INT NOT NULL,
    `internal_ip` VARCHAR(15) NOT NULL,
    `user_id` INT NOT NULL,
    `subdomain` VARCHAR(50) NOT NULL
);

CREATE TABLE volum_ssh_logs (
    `id` INT AUTO_INCREMENT PRIMARY KEY,
    `time_at` DATETIME DEFAULT NOW(),
    `vm_id` INT NOT NULL,
    `username` VARCHAR(20) NOT NULL,
    `command` VARCHAR(10000)
);

INSERT INTO volum_status (status, version) VALUES ('online', 'pre-1.0.0');