CREATE TABLE hosts (
    host_id SERIAL PRIMARY KEY,
    host_name VARCHAR(100) UNIQUE NOT NULL
);

CREATE TABLE users (
    user_id SERIAL PRIMARY KEY,
	host_id INTEGER REFERENCES hosts(host_id),
    user_name VARCHAR(100) NOT NULL,
    password_hash TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE mailBodies (
    mail_body_id SERIAL PRIMARY KEY,
    body_content TEXT NOT NULL
);

CREATE TABLE emailMessages (
    email_message_id SERIAL PRIMARY KEY,
    sender_id INTEGER REFERENCES users(user_id),
    recipient_id INTEGER REFERENCES users(user_id),
    subject VARCHAR(255),
	mail_body_id INTEGER REFERENCES mailBodies(mail_body_id),
    sent_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    is_received BOOLEAN DEFAULT FALSE
);

--Insert dummy data
INSERT INTO mailBodies (body_content) VALUES
('This is the body of the first email.'),
('This is the body of the second email.'),
('This is the body of the third email.');

INSERT INTO hosts (host_name) VALUES ('host1'), ('host2');

INSERT INTO users (host_id, user_name, password_hash) VALUES 
(1, 'user1', 'password_hash1'),
(1, 'user2', 'password_hash2'),
(1, 'user3', 'password_hash3');

INSERT INTO emailMessages (sender_id, recipient_id, subject, mail_body_id, is_received) VALUES
(1, 2, 'Subject 1', 1, TRUE),
(2, 1, 'Subject 2', 2, FALSE),
(3, 1, 'Subject 3', 3, TRUE);
