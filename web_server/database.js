import mysql from 'mysql2'
import dotenv from 'dotenv'
dotenv.config()

const pool = mysql.createPool({
    host: process.env.MYSQL_HOST,
    user: process.env.MYSQL_USER,
    password: process.env.MYSQL_PASSWORD,
    database: process.env.MYSQL_DATABASE
}).promise()

export async function getESPdata() {
    const [rows] = await pool.query("SELECT * FROM esp_data")
    return rows
}

export async function getESPonedata(id){
    const [rows] = await pool.query("SELECT * FROM esp_data  WHERE id = ?", [id])
    return rows[0]
}

export async function createEspData(title, value) {
    const [result] = await pool.query("INSERT INTO esp_data (title, value) VALUES (?, ?)", [title, value])
    const id = result.insertId
    return getESPonedata(id)
}