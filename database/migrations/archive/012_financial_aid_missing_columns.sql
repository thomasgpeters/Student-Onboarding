-- Migration: Add missing columns to financial_aid table
-- These columns are needed to properly store all form data

-- Add applying_for_aid boolean
ALTER TABLE financial_aid ADD COLUMN IF NOT EXISTS applying_for_aid BOOLEAN DEFAULT TRUE;

-- Add household_income_range to store the income range text (e.g., "$50,000 - $75,000")
ALTER TABLE financial_aid ADD COLUMN IF NOT EXISTS household_income_range VARCHAR(100);

-- Add special_circumstances for financial hardship notes
ALTER TABLE financial_aid ADD COLUMN IF NOT EXISTS special_circumstances TEXT;

-- Add veteran_benefits boolean
ALTER TABLE financial_aid ADD COLUMN IF NOT EXISTS veteran_benefits BOOLEAN DEFAULT FALSE;

-- Add work_study_interest boolean
ALTER TABLE financial_aid ADD COLUMN IF NOT EXISTS work_study_interest BOOLEAN DEFAULT FALSE;

-- Add loan_interest boolean
ALTER TABLE financial_aid ADD COLUMN IF NOT EXISTS loan_interest BOOLEAN DEFAULT FALSE;

-- Add scholarship_interest boolean
ALTER TABLE financial_aid ADD COLUMN IF NOT EXISTS scholarship_interest BOOLEAN DEFAULT FALSE;
