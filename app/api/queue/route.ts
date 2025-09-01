import { NextResponse } from 'next/server';
import { getQueue } from './state';

export async function GET() {
  return NextResponse.json(getQueue());
}
