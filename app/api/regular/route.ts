import { NextResponse } from 'next/server';
import { incrementRegular } from '../queue/state';

export async function POST() {
  return NextResponse.json(incrementRegular());
}
