-- Migration 003: Seed Sample Students
-- Adds sample student data for development and testing

-- Insert sample students with various statuses and programs
INSERT INTO student (
    email, password_hash, first_name, last_name, middle_name, preferred_name,
    preferred_pronouns, date_of_birth, gender, phone_number,
    curriculum_id, student_type, enrollment_date, expected_graduation,
    is_international, is_transfer_student, is_veteran, requires_financial_aid,
    intake_status, created_at
) VALUES
-- Active students in Computer Science
('john.doe@university.edu', '$2a$10$hashedpassword1', 'John', 'Doe', 'Michael', 'Johnny',
 'he/him', '2002-05-15', 'male', '555-0101',
 1, 'undergraduate', '2024-08-20', '2028-05-15',
 FALSE, FALSE, FALSE, TRUE,
 'in_progress', '2024-08-20 10:00:00'),

('jane.smith@university.edu', '$2a$10$hashedpassword2', 'Jane', 'Smith', 'Elizabeth', NULL,
 'she/her', '2001-11-22', 'female', '555-0102',
 1, 'undergraduate', '2024-08-20', '2028-05-15',
 FALSE, FALSE, FALSE, FALSE,
 'completed', '2024-08-18 14:30:00'),

('mike.johnson@university.edu', '$2a$10$hashedpassword3', 'Michael', 'Johnson', NULL, 'Mike',
 'he/him', '2003-03-08', 'male', '555-0103',
 2, 'graduate', '2024-08-25', '2026-05-15',
 FALSE, FALSE, FALSE, TRUE,
 'in_progress', '2024-08-25 09:15:00'),

-- Business Administration students
('sarah.wilson@university.edu', '$2a$10$hashedpassword4', 'Sarah', 'Wilson', 'Anne', NULL,
 'she/her', '2000-07-12', 'female', '555-0104',
 3, 'graduate', '2024-01-10', '2026-01-10',
 FALSE, FALSE, FALSE, FALSE,
 'completed', '2024-01-10 11:00:00'),

('david.brown@university.edu', '$2a$10$hashedpassword5', 'David', 'Brown', 'Lee', NULL,
 'he/him', '2002-09-30', 'male', '555-0105',
 4, 'undergraduate', '2024-08-20', '2028-05-15',
 FALSE, TRUE, FALSE, TRUE,
 'in_progress', '2024-08-19 16:45:00'),

-- Engineering students
('emily.chen@university.edu', '$2a$10$hashedpassword6', 'Emily', 'Chen', NULL, 'Em',
 'she/her', '2001-12-05', 'female', '555-0106',
 5, 'undergraduate', '2024-08-20', '2028-05-15',
 TRUE, FALSE, FALSE, FALSE,
 'in_progress', '2024-08-17 08:30:00'),

('alex.rodriguez@university.edu', '$2a$10$hashedpassword7', 'Alexander', 'Rodriguez', 'Miguel', 'Alex',
 'he/him', '2002-02-28', 'male', '555-0107',
 6, 'undergraduate', '2024-08-20', '2028-05-15',
 FALSE, FALSE, TRUE, TRUE,
 'in_progress', '2024-08-21 10:00:00'),

-- Science and Arts students
('jessica.taylor@university.edu', '$2a$10$hashedpassword8', 'Jessica', 'Taylor', 'Marie', 'Jess',
 'she/her', '2003-06-18', 'female', '555-0108',
 7, 'undergraduate', '2024-08-20', '2028-05-15',
 FALSE, FALSE, FALSE, FALSE,
 'completed', '2024-08-15 13:20:00'),

('ryan.martinez@university.edu', '$2a$10$hashedpassword9', 'Ryan', 'Martinez', NULL, NULL,
 'he/him', '2001-04-25', 'male', '555-0109',
 8, 'undergraduate', '2024-08-20', '2028-05-15',
 FALSE, TRUE, FALSE, TRUE,
 'in_progress', '2024-08-22 15:00:00'),

-- International student
('wei.zhang@university.edu', '$2a$10$hashedpassword10', 'Wei', 'Zhang', NULL, NULL,
 'they/them', '2002-10-10', 'non-binary', '555-0110',
 1, 'graduate', '2024-08-25', '2026-05-15',
 TRUE, FALSE, FALSE, FALSE,
 'in_progress', '2024-08-24 09:00:00'),

-- More students for variety
('olivia.anderson@university.edu', '$2a$10$hashedpassword11', 'Olivia', 'Anderson', 'Grace', 'Liv',
 'she/her', '2002-01-14', 'female', '555-0111',
 2, 'graduate', '2024-01-15', '2026-01-15',
 FALSE, FALSE, FALSE, TRUE,
 'completed', '2024-01-14 10:30:00'),

('noah.thomas@university.edu', '$2a$10$hashedpassword12', 'Noah', 'Thomas', 'James', NULL,
 'he/him', '2003-08-20', 'male', '555-0112',
 4, 'undergraduate', '2024-08-20', '2028-05-15',
 FALSE, FALSE, FALSE, FALSE,
 'in_progress', '2024-08-20 14:00:00');

-- Insert sample addresses for students
INSERT INTO student_address (student_id, address_type, street1, street2, city, state, postal_code, country, is_primary) VALUES
(1, 'permanent', '123 Main Street', 'Apt 4B', 'Springfield', 'IL', '62701', 'USA', TRUE),
(1, 'mailing', '456 Campus Drive', 'Dorm 101', 'University City', 'IL', '62702', 'USA', FALSE),
(2, 'permanent', '789 Oak Avenue', NULL, 'Riverside', 'CA', '92501', 'USA', TRUE),
(3, 'permanent', '321 Pine Road', 'Suite 100', 'Portland', 'OR', '97201', 'USA', TRUE),
(4, 'permanent', '555 Elm Street', NULL, 'Austin', 'TX', '78701', 'USA', TRUE),
(5, 'permanent', '888 Maple Lane', 'Unit 2A', 'Denver', 'CO', '80201', 'USA', TRUE),
(6, 'permanent', '999 Cedar Court', NULL, 'Seattle', 'WA', '98101', 'USA', TRUE),
(7, 'permanent', '111 Birch Boulevard', NULL, 'Phoenix', 'AZ', '85001', 'USA', TRUE),
(8, 'permanent', '222 Willow Way', 'Apt 3C', 'Boston', 'MA', '02101', 'USA', TRUE),
(9, 'permanent', '333 Spruce Street', NULL, 'Miami', 'FL', '33101', 'USA', TRUE),
(10, 'permanent', '444 Redwood Road', NULL, 'San Francisco', 'CA', '94101', 'USA', TRUE),
(11, 'permanent', '666 Aspen Avenue', 'Floor 2', 'Chicago', 'IL', '60601', 'USA', TRUE),
(12, 'permanent', '777 Palm Place', NULL, 'San Diego', 'CA', '92101', 'USA', TRUE);

-- Insert emergency contacts for some students
INSERT INTO emergency_contact (student_id, first_name, last_name, contact_relationship, phone, email, is_primary, priority) VALUES
(1, 'Robert', 'Doe', 'father', '555-1001', 'robert.doe@email.com', TRUE, 1),
(1, 'Mary', 'Doe', 'mother', '555-1002', 'mary.doe@email.com', FALSE, 2),
(2, 'Thomas', 'Smith', 'father', '555-1003', 'thomas.smith@email.com', TRUE, 1),
(3, 'Linda', 'Johnson', 'mother', '555-1004', 'linda.johnson@email.com', TRUE, 1),
(4, 'James', 'Wilson', 'spouse', '555-1005', 'james.wilson@email.com', TRUE, 1),
(5, 'Patricia', 'Brown', 'mother', '555-1006', 'patricia.brown@email.com', TRUE, 1),
(6, 'Li', 'Chen', 'father', '555-1007', 'li.chen@email.com', TRUE, 1);

-- Insert some form submissions for students
INSERT INTO form_submission (student_id, form_type_id, status, submitted_at, created_at) VALUES
-- John Doe - some forms submitted
(1, 1, 'approved', '2024-08-20 11:00:00', '2024-08-20 10:30:00'),
(1, 2, 'pending', NULL, '2024-08-21 09:00:00'),
-- Jane Smith - all forms completed
(2, 1, 'approved', '2024-08-18 15:00:00', '2024-08-18 14:30:00'),
(2, 2, 'approved', '2024-08-19 10:00:00', '2024-08-18 16:00:00'),
(2, 7, 'approved', '2024-08-19 11:00:00', '2024-08-19 10:30:00'),
-- Mike Johnson
(3, 1, 'pending', NULL, '2024-08-25 10:00:00'),
-- Sarah Wilson - completed
(4, 1, 'approved', '2024-01-10 12:00:00', '2024-01-10 11:30:00'),
(4, 2, 'approved', '2024-01-11 09:00:00', '2024-01-10 14:00:00'),
(4, 4, 'approved', '2024-01-12 10:00:00', '2024-01-11 15:00:00'),
-- David Brown
(5, 1, 'approved', '2024-08-19 17:00:00', '2024-08-19 16:45:00'),
(5, 5, 'pending', NULL, '2024-08-20 10:00:00'),
-- Emily Chen
(6, 1, 'pending', NULL, '2024-08-17 09:00:00'),
(6, 6, 'pending', NULL, '2024-08-18 10:00:00');
