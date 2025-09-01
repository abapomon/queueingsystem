import { NextResponse } from 'next/server';
import { incrementSenior } from '../queue/state';

export async function POST() {
  return NextResponse.json(incrementSenior());
}
